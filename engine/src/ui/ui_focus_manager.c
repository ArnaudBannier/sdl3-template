/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_focus_manager.h"

UIFocusManager* UIFocusManager_create(UISystem* uiSystem)
{
    UIFocusManager* self = (UIFocusManager*)calloc(1, sizeof(UIFocusManager));
    ASSERT_NEW(self);

    self->uiSystem = uiSystem;
    self->enabled = true;
    self->elementCount = 0;
    self->focused = NULL;

    return self;
}

void UIFocusManager_destroy(UIFocusManager* self)
{
    if (!self) return;
    free(self);
}

void UIFocusManager_addSelectable(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");
    assert(selectable && "selectable must not be NULL");

    if (self->elementCount >= MAX_UI_SELECTABLES_IN_GROUP)
    {
        assert(false && "Maximum number of selectables reached");
        return;
    }

    const UIObjectId id = UIObject_getObjectId(selectable);

    for (int i = 0; i < self->elementCount; i++)
    {
        UIFocusManagerElement* element = &self->elements[i];
        if (UIObjectId_equal(element->id, id))
        {
            printf("Selectable is already in the focus manager\n");
            return;
        }
    }
    self->elements[self->elementCount].id = id;
    self->elements[self->elementCount].selectable = selectable;
    self->elementCount++;
}

void UIFocusManager_removeSelectable(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");
    assert(selectable && "selectable must not be NULL");

    if (self->focused == selectable)
    {
        self->focused = NULL;
    }

    const UIObjectId id = UIObject_getObjectId(selectable);

    for (int i = 0; i < self->elementCount; i++)
    {
        UIFocusManagerElement* element = &self->elements[i];
        if (UIObjectId_equal(element->id, id))
        {
            self->elements[i] = self->elements[self->elementCount - 1];
            self->elementCount--;
            break;
        }
    }
}

void UIFocusManager_clear(UIFocusManager* self)
{
    assert(self && "self must not be NULL");
    self->elementCount = 0;
    self->focused = NULL;
}

static void UIFocusManager_setFocusOn(UIFocusManager* self, UISelectable* selectable)
{
    if (self->focused == selectable) return;
    if (self->focused)
    {
        UISelectable_setFocusState(self->focused, UI_FOCUS_STATE_NORMAL);

        if (selectable)
        {
            int audioId = UISelectable_getAudioOnFocused(selectable);
            UISystem_playSFX(self->uiSystem, audioId);
        }
    }
    self->focused = selectable;
    UISelectable_setFocusState(self->focused, UI_FOCUS_STATE_FOCUSED);
}

void UIFocusManager_setFocused(UIFocusManager* self, void* selectable)
{
    assert(self && "self must not be NULL");

    if (self->focused && self->focused == selectable) return;

    const UIObjectId id = UIObject_getObjectId(selectable);
    for (int i = 0; i < self->elementCount; i++)
    {
        UIFocusManagerElement* element = &self->elements[i];
        if (!UIObjectId_equal(element->id, id)) continue;
        UIFocusManager_setFocusOn(self, element->selectable);
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
    UIObject_getAABB(self->focused, &currAABB);

    UISelectable* bestNext = NULL;
    AABB bestNextAABB = { 0 };
    float bestDistance = INFINITY;

    for (int i = 0; i < self->elementCount; i++)
    {
        UIFocusManagerElement* element = &self->elements[i];
        UISelectable* next = element->selectable;
        if (next == self->focused) continue;
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
    UISystem* uiSystem = self->uiSystem;
    Vec2 mouseUIPos = input->mouseUIPos;
    AABB aabb = { 0 };

    if (input->mouseMoved)
    {
        for (int i = 0; i < self->elementCount; i++)
        {
            UIFocusManagerElement* element = &self->elements[i];
            UISelectable* selectable = element->selectable;
            UIFocusState state = UISelectable_getFocusState(selectable);
            if (state == UI_FOCUS_STATE_DISABLED) continue;

            UIObject_getAABB(selectable, &aabb);
            if (AABB_containsPoint(&aabb, mouseUIPos) == false) continue;

            if (self->focused == selectable) break;

            UIFocusManager_setFocusOn(self, selectable);
            break;
        }
    }
    if (self->focused && input->lastInputType == UI_INPUT_TYPE_MOUSE)
    {
        UISelectable_onFocus(self->focused, input);
    }
}

static void UIFocusManager_updateGamepad(UIFocusManager* self, UIInput* input)
{
    int navFlags = 0;
    if (input->upPressed) navFlags |= UI_ACTION_UP;
    if (input->downPressed) navFlags |= UI_ACTION_DOWN;
    if (input->leftPressed) navFlags |= UI_ACTION_LEFT;
    if (input->rightPressed) navFlags |= UI_ACTION_RIGHT;

    if (self->focused && input->lastInputType != UI_INPUT_TYPE_MOUSE)
    {
        UISelectable_onFocus(self->focused, input);

        if ((UISelectable_shouldHandleAction(self->focused, navFlags) == false) && (navFlags != 0))
        {
            UIFocusManager_searchNext(self, input);
        }
    }
}

static void UIFocusManager_validate(UIFocusManager* self)
{
    UISystem* uiSystem = self->uiSystem;
    for (int i = 0; i < self->elementCount;)
    {
        UIFocusManagerElement* element = &self->elements[i];
        if (UISystem_hasObjectId(uiSystem, element->id) == false)
        {
            SDL_LogWarn(
                SDL_LOG_CATEGORY_SYSTEM,
                "Selectable not found in canvas, removing from focus manager\n"
            );
            self->elements[i] = self->elements[self->elementCount - 1];
            self->elementCount--;
            continue;
        }
        i++;
    }

    bool focusedIsValid = false;
    int firstValidIndex = -1;
    for (int i = 0; i < self->elementCount; i++)
    {
        UISelectable* selectable = self->elements[i].selectable;
        UIFocusState state = UISelectable_getFocusState(selectable);
        if (state == UI_FOCUS_STATE_DISABLED) continue;

        if (firstValidIndex < 0) firstValidIndex = i;

        if (selectable == self->focused)
        {
            focusedIsValid = true;
            continue;
        }
        UISelectable_setFocusState(selectable, UI_FOCUS_STATE_NORMAL);
    }

    if (!focusedIsValid) self->focused = NULL;

    if (self->focused == NULL && firstValidIndex >= 0)
    {
        UIFocusManager_setFocusOn(self, self->elements[firstValidIndex].selectable);
    }
}

void UIFocusManager_update(UIFocusManager* self, UIInput* input)
{
    assert(self && "self must not be NULL");
    assert(input && "input must not be NULL");

    if (!self->enabled) return;
    if (self->elementCount == 0) return;

    UIFocusManager_validate(self);
    if (!self->focused) return;

    UIFocusManager_updateMouse(self, input);
    UIFocusManager_updateGamepad(self, input);
}
