/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_focus_manager.h"

UIFocusManager* UIFocusManager_create()
{
    UIFocusManager* self = (UIFocusManager*)calloc(1, sizeof(UIFocusManager));
    AssertNew(self);

    self->m_enabled = true;
    self->m_elementCount = 0;
    self->m_focused = NULL;

    return self;
}

void UIFocusManager_destroy(UIFocusManager* self)
{
    if (!self) return;
    free(self);
}

void UIFocusManager_setCanvas(UIFocusManager* self, void* canvas)
{
    assert(self && "self must not be NULL");
    if (canvas)
    {
        assert(UIObject_isOfType(canvas, UI_TYPE_CANVAS) && "canvas must be of type UI_TYPE_CANVAS");
    }
    self->m_canvas = (UICanvas*)canvas;
}

void UIFocusManager_addSelectable(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");
    assert(selectable && "selectable must not be NULL");

    if (self->m_elementCount >= MAX_UI_SELECTABLES_IN_GROUP)
    {
        assert(false && "Maximum number of selectables reached");
        return;
    }

    const int id = UIObject_getObjectId(selectable);

    for (int i = 0; i < self->m_elementCount; i++)
    {
        UIFocusManagerElement* element = &self->m_elements[i];
        if (element->m_id == id)
        {
            printf("Selectable is already in the focus manager\n");
            return;
        }
    }
    self->m_elements[self->m_elementCount].m_id = id;
    self->m_elements[self->m_elementCount].m_selectable = selectable;
    self->m_elementCount++;
}

void UIFocusManager_removeSelectable(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");
    assert(selectable && "selectable must not be NULL");

    if (self->m_focused == selectable)
    {
        self->m_focused = NULL;
    }

    const int id = UIObject_getObjectId(selectable);

    for (int i = 0; i < self->m_elementCount; i++)
    {
        UIFocusManagerElement* element = &self->m_elements[i];
        if (element->m_id == id)
        {
            self->m_elements[i] = self->m_elements[self->m_elementCount - 1];
            self->m_elementCount--;
            break;
        }
    }
}

void UIFocusManager_clear(UIFocusManager* self)
{
    assert(self && "self must not be NULL");
    self->m_elementCount = 0;
    self->m_focused = NULL;
}

static void UIFocusManager_setFocusOn(UIFocusManager* self, UISelectable* selectable)
{
    if (self->m_focused == selectable) return;
    if (self->m_focused)
    {
        UISelectable_setFocusState(self->m_focused, UI_FOCUS_STATE_NORMAL);
    }
    self->m_focused = selectable;
    UISelectable_setFocusState(self->m_focused, UI_FOCUS_STATE_FOCUSED);
}

void UIFocusManager_setFocused(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");

    if (self->m_focused && self->m_focused == selectable) return;

    const int id = UIObject_getObjectId(selectable);
    for (int i = 0; i < self->m_elementCount; i++)
    {
        UIFocusManagerElement* element = &self->m_elements[i];
        if (element->m_id != id) continue;

        UIFocusManager_setFocusOn(self, element->m_selectable);
        return;
    }

    assert(false && "selectable not found in focus manager");
}

static void UIFocusManager_searchNext(UIFocusManager* self, UIInput* input)
{
    const float kPenaltyFactor = 2.f;

    Vec2 direction = { 0, 0 };
    direction.x = (float)(input->rightPressed - input->leftPressed);
    direction.y = (float)(input->upPressed - input->downPressed);
    direction = Vec2_normalize(direction);
    Vec2 directionPerp = Vec2_perp(direction);

    AABB currAABB = { 0 };
    UIObject_getAABB(self->m_focused, &currAABB);

    UISelectable* bestNext = NULL;
    AABB bestNextAABB = { 0 };
    float bestDistance = INFINITY;

    for (int i = 0; i < self->m_elementCount; i++)
    {
        UIFocusManagerElement* element = &self->m_elements[i];
        UISelectable* next = element->m_selectable;
        if (next == self->m_focused) continue;
        if (UISelectable_getFocusState(next) == UI_FOCUS_STATE_DISABLED) continue;

        AABB nextAABB = { 0 };
        UIObject_getAABB(next, &nextAABB);

        Vec2 shortestVector = AABB_shortestVector(&currAABB, &nextAABB);

        float proj = Vec2_dot(shortestVector, direction);
        if (proj <= 0.f) continue;

        float perpProj = Vec2_dot(shortestVector, directionPerp);
        float distance = proj + fabsf(perpProj) * kPenaltyFactor;

        if ((distance < bestDistance) ||
            ((fabsf(distance - bestDistance) < 1e-5) && (nextAABB.lower.x < bestNextAABB.lower.x)))
        {
            bestDistance = distance;
            bestNext = next;
            bestNextAABB = nextAABB;
        }
    }

    if (bestNext)
    {
        UIFocusManager_setFocusOn(self, bestNext);
    }
}

static void UIFocusManager_updateMouse(UIFocusManager* self, UIInput* input)
{
    SDL_FPoint mousePoint = { input->mousePxPos.x, input->mousePxPos.y };
    Vec2 mouseUIPos = input->mouseUIPos;
    AABB aabb = { 0 };

    if (input->mouseMoved)
    {
        for (int i = 0; i < self->m_elementCount; i++)
        {
            UIFocusManagerElement* element = &self->m_elements[i];
            UISelectable* selectable = element->m_selectable;
            UIFocusState state = UISelectable_getFocusState(selectable);
            if (state == UI_FOCUS_STATE_DISABLED) continue;

            UIObject_getAABB(selectable, &aabb);
            if (AABB_containsPoint(&aabb, mouseUIPos) == false) continue;

            if (self->m_focused == selectable) break;

            UIFocusManager_setFocusOn(self, selectable);
            break;
        }
    }
    if (self->m_focused && input->lastInputType == UI_INPUT_TYPE_MOUSE)
    {
        UISelectable_onFocus(self->m_focused, input);
    }
}

static void UIFocusManager_updateGamepad(UIFocusManager* self, UIInput* input)
{
    int navFlags = 0;
    if (input->upPressed) navFlags |= UI_ACTION_UP;
    if (input->downPressed) navFlags |= UI_ACTION_DOWN;
    if (input->leftPressed) navFlags |= UI_ACTION_LEFT;
    if (input->rightPressed) navFlags |= UI_ACTION_RIGHT;

    if (self->m_focused && input->lastInputType != UI_INPUT_TYPE_MOUSE)
    {
        UISelectable_onFocus(self->m_focused, input);

        if ((UISelectable_shouldHandleAction(self->m_focused, navFlags) == false) && (navFlags != 0))
        {
            UIFocusManager_searchNext(self, input);
        }
    }
}

static void UIFocusManager_validate(UIFocusManager* self)
{
    if (self->m_canvas)
    {
        for (int i = 0; i < self->m_elementCount;)
        {
            UIFocusManagerElement* element = &self->m_elements[i];
            if (UICanvas_hasObject(self->m_canvas, element->m_id) == false)
            {
                SDL_LogWarn(
                    SDL_LOG_CATEGORY_SYSTEM,
                    "Selectable with id %d not found in canvas, removing from focus manager\n",
                    element->m_id
                );
                self->m_elements[i] = self->m_elements[self->m_elementCount - 1];
                self->m_elementCount--;
                continue;
            }
            i++;
        }
    }

    bool focusedIsValid = false;
    int firstValidIndex = -1;
    for (int i = 0; i < self->m_elementCount; i++)
    {
        UISelectable* selectable = self->m_elements[i].m_selectable;
        UIFocusState state = UISelectable_getFocusState(selectable);
        if (state == UI_FOCUS_STATE_DISABLED) continue;

        if (firstValidIndex < 0) firstValidIndex = i;

        if (selectable == self->m_focused)
        {
            focusedIsValid = true;
            continue;
        }
        UISelectable_setFocusState(selectable, UI_FOCUS_STATE_NORMAL);
    }

    if (!focusedIsValid) self->m_focused = NULL;

    if (self->m_focused == NULL && firstValidIndex >= 0)
    {
        UIFocusManager_setFocusOn(self, self->m_elements[firstValidIndex].m_selectable);
    }
}

void UIFocusManager_update(UIFocusManager* self, UIInput* input)
{
    assert(self && "self must not be NULL");
    assert(input && "input must not be NULL");

    if (!self->m_enabled) return;
    if (self->m_elementCount == 0) return;

    UIFocusManager_validate(self);
    if (!self->m_focused) return;

    UIFocusManager_updateMouse(self, input);
    UIFocusManager_updateGamepad(self, input);
}
