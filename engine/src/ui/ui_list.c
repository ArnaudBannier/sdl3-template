/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_list.h"
#include "ui/ui_system.h"
#include "engine_common.h"

static void UIList_nextItem(UIList* self)
{
    int prevIdx = self->itemIdx;
    if ((self->configFlags & UI_LIST_CONFIG_CYCLE) != 0)
    {
        self->itemIdx = (self->itemIdx + 1) % self->itemCount;
    }
    else
    {
        self->itemIdx = Int_clamp(self->itemIdx + 1, 0, self->itemCount - 1);
    }
    if (self->itemIdx != prevIdx)
    {
        self->onItemChanged(self, self->itemIdx, prevIdx, true);
        if (self->userOnItemChanged)
        {
            self->userOnItemChanged(self, self->itemIdx, prevIdx, true);
        }
    }
}

static void UIList_prevItem(UIList* self)
{
    int prevIdx = self->itemIdx;
    if ((self->configFlags & UI_LIST_CONFIG_CYCLE) != 0)
    {
        self->itemIdx = (self->itemIdx - 1 + self->itemCount) % self->itemCount;
    }
    else
    {
        self->itemIdx = Int_clamp(self->itemIdx - 1, 0, self->itemCount - 1);
    }
    if (self->itemIdx != prevIdx)
    {
        self->onItemChanged(self, self->itemIdx, prevIdx, false);
        if (self->userOnItemChanged)
        {
            self->userOnItemChanged(self, self->itemIdx, prevIdx, false);
        }
    }
}

static void UIList_prevButtonCB(void* button)
{
    assert(button && "button must not be NULL");
    UIList* selfList = (UIList*)UIObject_getParent(button);
    UIList_prevItem(selfList);
}

static void UIList_nextButtonCB(void* button)
{
    assert(button && "button must not be NULL");
    UIList* selfList = (UIList*)UIObject_getParent(button);
    UIList_nextItem(selfList);
}

static void UIList_updateHandledAction(UIList* self)
{
    UISelectable* selfSelectable = (UISelectable*)self;
    UIList* selfList = (UIList*)self;
    if (selfSelectable->focusState == UI_FOCUS_STATE_DISABLED)
    {
        selfSelectable->handledActionFlags = 0;
        return;
    }

    if (self->configFlags & UI_LIST_CONFIG_AUTO_NAVIGATION)
    {
        selfSelectable->handledActionFlags =
            UI_ACTION_LEFT | UI_ACTION_RIGHT | UI_ACTION_VALIDATE | UI_ACTION_CLICK;
    }
    else
    {
        if (selfList->isActive)
        {
            selfSelectable->handledActionFlags =
                UI_ACTION_LEFT | UI_ACTION_RIGHT | UI_ACTION_CANCEL | UI_ACTION_VALIDATE | UI_ACTION_CLICK;
        }
        else
        {
            selfSelectable->handledActionFlags = UI_ACTION_VALIDATE | UI_ACTION_CLICK;
        }
    }
}

UIList* UIList_create(
    UISystem* uiSystem, const char* objectName,
    TTF_Font* font, int itemCount, int configFlags)
{
    UIList* self = (UIList*)calloc(1, sizeof(UIList));
    ASSERT_NEW(self);

    UIList_init(self, uiSystem, objectName, font, itemCount, configFlags);

    return self;
}

void UIList_init(
    void* selfPtr, UISystem* uiSystem, const char* objectName,
    TTF_Font* font, int itemCount, int configFlags)
{
    assert(selfPtr && "self must not be NULL");

    UISelectable_init(selfPtr, uiSystem, objectName);
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;

    selfObj->type |= UI_TYPE_LIST;
    selfList->itemCount = itemCount;
    selfList->itemIdx = 0;
    selfList->configFlags = configFlags;
    selfList->useColorMod = false;

    selfList->labelText.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "", 0);
    selfList->labelText.color = g_colors.gray0;
    selfList->labelText.anchor = Vec2_anchor_west;
    ASSERT_NEW(selfList->labelText.ttfText);

    selfList->itemText.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "", 0);
    selfList->itemText.color = g_colors.gray0;
    selfList->itemText.anchor = Vec2_anchor_center;
    ASSERT_NEW(selfList->itemText.ttfText);

    SDL_Color defaultTextColors[UI_LIST_STATE_COUNT] = { 0 };
    SDL_Color defaultBackColors[UI_LIST_STATE_COUNT] = { 0 };

    defaultTextColors[UI_LIST_STATE_NORMAL] = (SDL_Color){ 200, 200, 200, 255 };
    defaultTextColors[UI_LIST_STATE_FOCUSED] = (SDL_Color){ 255, 255, 255, 255 };
    defaultTextColors[UI_LIST_STATE_ACTIVE] = (SDL_Color){ 255, 255, 255, 255 };
    defaultTextColors[UI_LIST_STATE_DISABLED] = (SDL_Color){ 128, 128, 128, 255 };

    defaultBackColors[UI_LIST_STATE_NORMAL] = (SDL_Color){ 50, 50, 50, 255 };
    defaultBackColors[UI_LIST_STATE_FOCUSED] = (SDL_Color){ 70, 70, 70, 255 };
    defaultBackColors[UI_LIST_STATE_ACTIVE] = (SDL_Color){ 80, 80, 80, 255 };
    defaultBackColors[UI_LIST_STATE_DISABLED] = (SDL_Color){ 30, 30, 30, 255 };

    // Initialize text contents and colors
    selfList->labelString = SDL_strdup("Label");
    selfList->itemStrings = SDL_malloc(itemCount * sizeof(char*));
    for (int i = 0; i < itemCount; i++)
    {
        char buffer[32] = { 0 };
        SDL_snprintf(buffer, sizeof(buffer), "Item %d", i);
        selfList->itemStrings[i] = SDL_strdup(buffer);
    }
    for (int i = 0; i < UI_LIST_STATE_COUNT; i++)
    {
        selfList->itemColors[i] = defaultTextColors[i];
        selfList->labelColors[i] = defaultTextColors[i];
        selfList->backColors[i] = defaultBackColors[i];
        selfList->spriteIndices[i] = -1;
    }

    // Buttons
    selfList->prevButton = UIButton_create(uiSystem, "UIList_PrevButton", font);
    selfList->nextButton = UIButton_create(uiSystem, "UIList_NextButton", font);
    UIObject_setParent(selfList->prevButton, selfList);
    UIObject_setParent(selfList->nextButton, selfList);
    UIButton_setLabelString(selfList->prevButton, "<");
    UIButton_setLabelString(selfList->nextButton, ">");
    UIButton_setOnClickCallback(selfList->prevButton, UIList_prevButtonCB);
    UIButton_setOnClickCallback(selfList->nextButton, UIList_nextButtonCB);

    UIButton* buttons[] = {
        selfList->prevButton,
        selfList->nextButton
    };
    for (int i = 0; i < 2; i++)
    {
        UIButton_setLabelColor(buttons[i], UI_BUTTON_STATE_NORMAL, defaultTextColors[UI_LIST_STATE_NORMAL]);
        UIButton_setLabelColor(buttons[i], UI_BUTTON_STATE_FOCUSED, defaultTextColors[UI_LIST_STATE_FOCUSED]);
        UIButton_setLabelColor(buttons[i], UI_BUTTON_STATE_PRESSED, defaultTextColors[UI_LIST_STATE_FOCUSED]);
        UIButton_setLabelColor(buttons[i], UI_BUTTON_STATE_DISABLED, defaultTextColors[UI_LIST_STATE_DISABLED]);
        UIButton_setBackgroundColor(buttons[i], UI_BUTTON_STATE_NORMAL, (SDL_Color) { 255, 255, 255, 0 });
        UIButton_setBackgroundColor(buttons[i], UI_BUTTON_STATE_FOCUSED, (SDL_Color) { 255, 255, 255, 31 });
        UIButton_setBackgroundColor(buttons[i], UI_BUTTON_STATE_PRESSED, (SDL_Color) { 0, 0, 0, 63 });
        UIButton_setBackgroundColor(buttons[i], UI_BUTTON_STATE_DISABLED, (SDL_Color) { 0, 0, 0, 0 });
        UIButton_setUseColorMod(buttons[i], false);
    }

    UIRect rect = { 0 };
    rect.anchorMin = Vec2_set(0.5f, 0.f);
    rect.anchorMax = Vec2_set(0.6f, 1.f);
    rect.offsetMin = Vec2_set(+3.f, +3.f);
    rect.offsetMax = Vec2_set(-3.f, -3.f);
    UIObject_setRect(selfList->prevButton, rect);

    rect.anchorMin = Vec2_set(0.9f, 0.f);
    rect.anchorMax = Vec2_set(1.0f, 1.f);
    rect.offsetMin = Vec2_set(+3.f, +3.f);
    rect.offsetMax = Vec2_set(-3.f, -3.f);
    UIObject_setRect(selfList->nextButton, rect);

    rect.anchorMin = Vec2_set(0.0f, 0.f);
    rect.anchorMax = Vec2_set(0.5f, 1.f);
    rect.offsetMin = Vec2_set(+5.f, +5.f);
    rect.offsetMax = Vec2_set(-5.f, -5.f);
    selfList->labelTransform.localRect = rect;

    rect.anchorMin = Vec2_set(0.5f, 0.f);
    rect.anchorMax = Vec2_set(1.0f, 1.f);
    rect.offsetMin = Vec2_set(+5.f, +5.f);
    rect.offsetMax = Vec2_set(-5.f, -5.f);
    selfList->itemTransform.localRect = rect;

    UIList_updateHandledAction(selfList);

    // Virtual methods
    selfObj->onDestroy = UIListVM_onDestroy;
    selfObj->onRender = UIListVM_onRender;
    selfObj->onUpdate = UIListVM_onUpdate;
    selfObj->onDrawGizmos = UIListVM_onDrawGizmos;
    selfSelectable->onFocusChanged = UIListVM_onFocusChanged;
    selfSelectable->onFocus = UIListVM_onFocus;
    selfList->onItemChanged = UIListVM_onItemChanged;
}

void UIListVM_onDestroy(void* selfPtr)
{
    UIList* selfList = (UIList*)selfPtr;

    SDL_free(selfList->labelString);
    for (int i = 0; i < selfList->itemCount; i++)
    {
        SDL_free(selfList->itemStrings[i]);
    }
    SDL_free(selfList->itemStrings);

    TTF_DestroyText(selfList->labelText.ttfText);
    TTF_DestroyText(selfList->itemText.ttfText);

    UISelectableVM_onDestroy(selfPtr);
}


static void UIList_updateListState(void* selfPtr)
{
    UIFocusState focusState = ((UISelectable*)selfPtr)->focusState;
    UIList* selfList = (UIList*)selfPtr;
    if (focusState == UI_FOCUS_STATE_DISABLED)
    {
        selfList->listState = UI_LIST_STATE_DISABLED;
        return;
    }

    if (selfList->isActive)
    {
        selfList->listState = UI_LIST_STATE_ACTIVE;
        return;
    }

    switch (focusState)
    {
    default:
    case UI_FOCUS_STATE_NORMAL:
        selfList->listState = UI_LIST_STATE_NORMAL;
        return;

    case UI_FOCUS_STATE_FOCUSED:
        selfList->listState = UI_LIST_STATE_FOCUSED;
        return;
    }
}

void UIListVM_onUpdate(void* selfPtr)
{
    UISelectableVM_onUpdate(selfPtr);
    UIObject* selfObj = (UIObject*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;
    UIFocusState focusState = UISelectable_getFocusState(selfPtr);

    selfList->hasPrevItem = true;
    selfList->hasNextItem = true;
    if ((selfList->configFlags & UI_LIST_CONFIG_CYCLE) == 0)
    {
        selfList->hasPrevItem = selfList->itemIdx > 0;
        selfList->hasNextItem = selfList->itemIdx < selfList->itemCount - 1;
    }
    if (selfList->hasPrevItem == false)
    {
        UISelectable_setFocusState(selfList->prevButton, UI_FOCUS_STATE_DISABLED);
    }
    if (selfList->hasNextItem == false)
    {
        UISelectable_setFocusState(selfList->nextButton, UI_FOCUS_STATE_DISABLED);
    }

    UIList_updateListState(selfPtr);
    UIList_updateHandledAction(selfList);

    UITransform_updateAABB(&selfList->labelTransform, &selfObj->transform);
    UITransform_updateAABB(&selfList->itemTransform, &selfObj->transform);

    // Update text
    bool success = TTF_SetTextString(
        selfList->labelText.ttfText,
        selfList->labelString,
        0
    );
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_SYSTEM);

    success = TTF_SetTextString(
        selfList->itemText.ttfText,
        selfList->itemStrings[selfList->itemIdx],
        0
    );
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_SYSTEM);
}

void UIListVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState)
{
    UISelectableVM_onFocusChanged(selfPtr, currState, prevState);
    UIList* selfList = (UIList*)selfPtr;

    if (currState == UI_FOCUS_STATE_NORMAL)
    {
        selfList->isActive = false;
        UISelectable_setFocusState(
            selfList->prevButton,
            selfList->hasPrevItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
        );
        UISelectable_setFocusState(
            selfList->nextButton,
            selfList->hasNextItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
        );
    }
}

void UIListVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;
    bool success = true;

    UIListState state = selfList->listState;

    SDL_Color backColor = selfList->backColors[state];
    selfList->labelText.color = selfList->labelColors[state];
    selfList->itemText.color = selfList->itemColors[state];

    Transform transform = { 0 };
    RenderDim dim = { 0 };
    RenderAnchor anchor = { 0 };
    RenderTexture texture = { 0 };
    RenderRect rect = { 0 };
    RenderColorMod colorMod = { 0 };
    GraphicsCmd cmd = {
        .sortingLayer = &(selfObj->layer),
        .transform = &transform,
        .dimensions = &dim,
        .anchor = &anchor,
    };


    // Background
    if (selfList->spriteGroup && selfList->spriteIndices[state] >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfList->spriteGroup,
            selfList->spriteIndices[state]
        );
        cmd.texture = &texture;
    }
    else
    {
        rect.color = backColor;
        rect.filled = true;
        cmd.rect = &rect;
    }

    if (selfList->useColorMod)
    {
        colorMod.r = backColor.r / 255.f;
        colorMod.g = backColor.g / 255.f;
        colorMod.b = backColor.b / 255.f;
        cmd.colorMod = &colorMod;
    }
    UITransform_getComponents(&(selfObj->transform), &transform, &dim, &anchor);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Label text
    UITransform_getComponents(&(selfList->labelTransform), &transform, &dim, &anchor);
    cmd.text = &(selfList->labelText);
    cmd.texture = NULL;
    cmd.rect = NULL;
    cmd.colorMod = NULL;
    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Item text
    UITransform_getComponents(&(selfList->itemTransform), &transform, &dim, &anchor);
    cmd.text = &(selfList->itemText);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

static void UIListVM_onMouseFocus(UIList* self, UIInput* input)
{
    Vec2 mouseUIPos = input->mouseUIPos;

    UIButton* buttons[] = {
        self->prevButton,
        self->nextButton
    };
    bool isEnabled[] = {
        self->hasPrevItem,
        self->hasNextItem
    };
    for (int i = 0; i < 2; i++)
    {
        UIButton* button = buttons[i];
        if (isEnabled[i] == false)
        {
            UISelectable_setFocusState(button, UI_FOCUS_STATE_DISABLED);
            continue;
        }

        AABB buttonAABB = { 0 };
        UIObject_getAABB(button, &buttonAABB);

        if (AABB_containsPoint(&buttonAABB, mouseUIPos) == true)
        {
            UISelectable_setFocusState(button, UI_FOCUS_STATE_FOCUSED);
            UISelectable_onFocus((UISelectable*)button, input);
        }
        else
        {
            UISelectable_setFocusState(button, UI_FOCUS_STATE_NORMAL);
        }
    }
}

static void UIListVM_onControllerFocus(UIList* self, UIInput* input)
{
    UISelectable_setFocusState(
        self->prevButton,
        self->hasPrevItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
    );
    UISelectable_setFocusState(
        self->nextButton,
        self->hasNextItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
    );

    if (self->configFlags & UI_LIST_CONFIG_AUTO_NAVIGATION)
    {
        if (input->leftPressed)
        {
            UIList_prevItem(self);
        }
        if (input->rightPressed)
        {
            UIList_nextItem(self);
        }
        return;
    }
    else
    {
        if (self->isActive)
        {
            if (input->leftPressed)
            {
                UIList_prevItem(self);
            }
            if (input->rightPressed)
            {
                UIList_nextItem(self);
            }
            if (input->cancelPressed)
            {
                self->isActive = false;
            }
        }
        else
        {
            if (input->validatePressed)
            {
                self->isActive = true;
            }
        }
    }
}

void UIListVM_onFocus(void* selfPtr, UIInput* input)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;

    if (input->lastInputType == UI_INPUT_TYPE_MOUSE)
    {
        UIListVM_onMouseFocus(selfList, input);
    }
    else
    {
        UIListVM_onControllerFocus(selfList, input);
    }

    UIObject_update(selfPtr);
}

void UIListVM_onItemChanged(void* selfPtr, int currItemIdx, int prevItemIdx, bool increase)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UISystem* uiSystem = UIObject_getUISystem(selfPtr);
    UISystem_playSFX(uiSystem, selfSelectable->audioOnClick);
}
