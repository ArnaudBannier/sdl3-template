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
    int prevIdx = self->m_itemIdx;
    if ((self->m_configFlags & UI_LIST_CONFIG_CYCLE) != 0)
    {
        self->m_itemIdx = (self->m_itemIdx + 1) % self->m_itemCount;
    }
    else
    {
        self->m_itemIdx = Int_clamp(self->m_itemIdx + 1, 0, self->m_itemCount - 1);
    }
    if (self->m_itemIdx != prevIdx)
    {
        self->m_onItemChanged(self, self->m_itemIdx, prevIdx, true);
        if (self->m_userOnItemChanged)
        {
            self->m_userOnItemChanged(self, self->m_itemIdx, prevIdx, true);
        }
    }
}

static void UIList_prevItem(UIList* self)
{
    int prevIdx = self->m_itemIdx;
    if ((self->m_configFlags & UI_LIST_CONFIG_CYCLE) != 0)
    {
        self->m_itemIdx = (self->m_itemIdx - 1 + self->m_itemCount) % self->m_itemCount;
    }
    else
    {
        self->m_itemIdx = Int_clamp(self->m_itemIdx - 1, 0, self->m_itemCount - 1);
    }
    if (self->m_itemIdx != prevIdx)
    {
        self->m_onItemChanged(self, self->m_itemIdx, prevIdx, false);
        if (self->m_userOnItemChanged)
        {
            self->m_userOnItemChanged(self, self->m_itemIdx, prevIdx, false);
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
    if (selfSelectable->m_focusState == UI_FOCUS_STATE_DISABLED)
    {
        selfSelectable->m_handledActionFlags = 0;
        return;
    }

    if (self->m_configFlags & UI_LIST_CONFIG_AUTO_NAVIGATION)
    {
        selfSelectable->m_handledActionFlags =
            UI_ACTION_LEFT | UI_ACTION_RIGHT | UI_ACTION_VALIDATE | UI_ACTION_CLICK;
    }
    else
    {
        if (selfList->m_isActive)
        {
            selfSelectable->m_handledActionFlags =
                UI_ACTION_LEFT | UI_ACTION_RIGHT | UI_ACTION_CANCEL | UI_ACTION_VALIDATE | UI_ACTION_CLICK;
        }
        else
        {
            selfSelectable->m_handledActionFlags = UI_ACTION_VALIDATE | UI_ACTION_CLICK;
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

    selfObj->m_type |= UI_TYPE_LIST;
    selfList->m_itemCount = itemCount;
    selfList->m_itemIdx = 0;
    selfList->m_configFlags = configFlags;
    selfList->m_useColorMod = false;

    selfList->m_labelText.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "", 0);
    selfList->m_labelText.color = g_colors.gray0;
    selfList->m_labelText.anchor = Vec2_anchor_west;
    ASSERT_NEW(selfList->m_labelText.ttfText);

    selfList->m_itemText.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "", 0);
    selfList->m_itemText.color = g_colors.gray0;
    selfList->m_itemText.anchor = Vec2_anchor_center;
    ASSERT_NEW(selfList->m_itemText.ttfText);

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
    selfList->m_labelString = SDL_strdup("Label");
    selfList->m_itemStrings = SDL_malloc(itemCount * sizeof(char*));
    for (int i = 0; i < itemCount; i++)
    {
        char buffer[32] = { 0 };
        SDL_snprintf(buffer, sizeof(buffer), "Item %d", i);
        selfList->m_itemStrings[i] = SDL_strdup(buffer);
    }
    for (int i = 0; i < UI_LIST_STATE_COUNT; i++)
    {
        selfList->m_itemColors[i] = defaultTextColors[i];
        selfList->m_labelColors[i] = defaultTextColors[i];
        selfList->m_backColors[i] = defaultBackColors[i];
        selfList->m_spriteIndices[i] = -1;
    }

    // Buttons
    selfList->m_prevButton = UIButton_create(uiSystem, "UIList_PrevButton", font);
    selfList->m_nextButton = UIButton_create(uiSystem, "UIList_NextButton", font);
    UIObject_setParent(selfList->m_prevButton, selfList);
    UIObject_setParent(selfList->m_nextButton, selfList);
    UIButton_setLabelString(selfList->m_prevButton, "<");
    UIButton_setLabelString(selfList->m_nextButton, ">");
    UIButton_setOnClickCallback(selfList->m_prevButton, UIList_prevButtonCB);
    UIButton_setOnClickCallback(selfList->m_nextButton, UIList_nextButtonCB);

    UIButton* buttons[] = {
        selfList->m_prevButton,
        selfList->m_nextButton
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
    UIObject_setRect(selfList->m_prevButton, rect);

    rect.anchorMin = Vec2_set(0.9f, 0.f);
    rect.anchorMax = Vec2_set(1.0f, 1.f);
    rect.offsetMin = Vec2_set(+3.f, +3.f);
    rect.offsetMax = Vec2_set(-3.f, -3.f);
    UIObject_setRect(selfList->m_nextButton, rect);

    rect.anchorMin = Vec2_set(0.0f, 0.f);
    rect.anchorMax = Vec2_set(0.5f, 1.f);
    rect.offsetMin = Vec2_set(+5.f, +5.f);
    rect.offsetMax = Vec2_set(-5.f, -5.f);
    selfList->m_labelTransform.localRect = rect;

    rect.anchorMin = Vec2_set(0.5f, 0.f);
    rect.anchorMax = Vec2_set(1.0f, 1.f);
    rect.offsetMin = Vec2_set(+5.f, +5.f);
    rect.offsetMax = Vec2_set(-5.f, -5.f);
    selfList->m_itemTransform.localRect = rect;

    UIList_updateHandledAction(selfList);

    // Virtual methods
    selfObj->m_onDestroy = UIListVM_onDestroy;
    selfObj->m_onRender = UIListVM_onRender;
    selfObj->m_onUpdate = UIListVM_onUpdate;
    selfObj->m_onDrawGizmos = UIListVM_onDrawGizmos;
    selfSelectable->m_onFocusChanged = UIListVM_onFocusChanged;
    selfSelectable->m_onFocus = UIListVM_onFocus;
    selfList->m_onItemChanged = UIListVM_onItemChanged;
}

void UIListVM_onDestroy(void* selfPtr)
{
    UIList* selfList = (UIList*)selfPtr;

    SDL_free(selfList->m_labelString);
    for (int i = 0; i < selfList->m_itemCount; i++)
    {
        SDL_free(selfList->m_itemStrings[i]);
    }
    SDL_free(selfList->m_itemStrings);

    TTF_DestroyText(selfList->m_labelText.ttfText);
    TTF_DestroyText(selfList->m_itemText.ttfText);

    UISelectableVM_onDestroy(selfPtr);
}


static void UIList_updateListState(void* selfPtr)
{
    UIFocusState focusState = ((UISelectable*)selfPtr)->m_focusState;
    UIList* selfList = (UIList*)selfPtr;
    if (focusState == UI_FOCUS_STATE_DISABLED)
    {
        selfList->m_listState = UI_LIST_STATE_DISABLED;
        return;
    }

    if (selfList->m_isActive)
    {
        selfList->m_listState = UI_LIST_STATE_ACTIVE;
        return;
    }

    switch (focusState)
    {
    default:
    case UI_FOCUS_STATE_NORMAL:
        selfList->m_listState = UI_LIST_STATE_NORMAL;
        return;

    case UI_FOCUS_STATE_FOCUSED:
        selfList->m_listState = UI_LIST_STATE_FOCUSED;
        return;
    }
}

void UIListVM_onUpdate(void* selfPtr)
{
    UISelectableVM_onUpdate(selfPtr);
    UIObject* selfObj = (UIObject*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;
    UIFocusState focusState = UISelectable_getFocusState(selfPtr);

    selfList->m_hasPrevItem = true;
    selfList->m_hasNextItem = true;
    if ((selfList->m_configFlags & UI_LIST_CONFIG_CYCLE) == 0)
    {
        selfList->m_hasPrevItem = selfList->m_itemIdx > 0;
        selfList->m_hasNextItem = selfList->m_itemIdx < selfList->m_itemCount - 1;
    }
    if (selfList->m_hasPrevItem == false)
    {
        UISelectable_setFocusState(selfList->m_prevButton, UI_FOCUS_STATE_DISABLED);
    }
    if (selfList->m_hasNextItem == false)
    {
        UISelectable_setFocusState(selfList->m_nextButton, UI_FOCUS_STATE_DISABLED);
    }

    UIList_updateListState(selfPtr);
    UIList_updateHandledAction(selfList);

    UITransform_updateAABB(&selfList->m_labelTransform, &selfObj->m_transform);
    UITransform_updateAABB(&selfList->m_itemTransform, &selfObj->m_transform);

    // Update text
    bool success = TTF_SetTextString(
        selfList->m_labelText.ttfText,
        selfList->m_labelString,
        0
    );
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_SYSTEM);

    success = TTF_SetTextString(
        selfList->m_itemText.ttfText,
        selfList->m_itemStrings[selfList->m_itemIdx],
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
        selfList->m_isActive = false;
        UISelectable_setFocusState(
            selfList->m_prevButton,
            selfList->m_hasPrevItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
        );
        UISelectable_setFocusState(
            selfList->m_nextButton,
            selfList->m_hasNextItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
        );
    }
}

void UIListVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIList* selfList = (UIList*)selfPtr;
    bool success = true;

    UIListState state = selfList->m_listState;

    SDL_Color backColor = selfList->m_backColors[state];
    selfList->m_labelText.color = selfList->m_labelColors[state];
    selfList->m_itemText.color = selfList->m_itemColors[state];

    Transform transform = { 0 };
    RenderDim dim = { 0 };
    RenderAnchor anchor = { 0 };
    RenderTexture texture = { 0 };
    RenderRect rect = { 0 };
    RenderColorMod colorMod = { 0 };
    GraphicsCmd cmd = {
        .sortingLayer = &(selfObj->m_layer),
        .transform = &transform,
        .dimensions = &dim,
        .anchor = &anchor,
    };


    // Background
    if (selfList->m_spriteGroup && selfList->m_spriteIndices[state] >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfList->m_spriteGroup,
            selfList->m_spriteIndices[state]
        );
        cmd.texture = &texture;
    }
    else
    {
        rect.color = backColor;
        rect.filled = true;
        cmd.rect = &rect;
    }

    if (selfList->m_useColorMod)
    {
        colorMod.r = backColor.r / 255.f;
        colorMod.g = backColor.g / 255.f;
        colorMod.b = backColor.b / 255.f;
        cmd.colorMod = &colorMod;
    }
    UITransform_getComponents(&(selfObj->m_transform), &transform, &dim, &anchor);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Label text
    UITransform_getComponents(&(selfList->m_labelTransform), &transform, &dim, &anchor);
    cmd.text = &(selfList->m_labelText);
    cmd.texture = NULL;
    cmd.rect = NULL;
    cmd.colorMod = NULL;
    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Item text
    UITransform_getComponents(&(selfList->m_itemTransform), &transform, &dim, &anchor);
    cmd.text = &(selfList->m_itemText);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

static void UIListVM_onMouseFocus(UIList* self, UIInput* input)
{
    Vec2 mouseUIPos = input->mouseUIPos;

    UIButton* buttons[] = {
        self->m_prevButton,
        self->m_nextButton
    };
    bool isEnabled[] = {
        self->m_hasPrevItem,
        self->m_hasNextItem
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
        self->m_prevButton,
        self->m_hasPrevItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
    );
    UISelectable_setFocusState(
        self->m_nextButton,
        self->m_hasNextItem ? UI_FOCUS_STATE_NORMAL : UI_FOCUS_STATE_DISABLED
    );

    if (self->m_configFlags & UI_LIST_CONFIG_AUTO_NAVIGATION)
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
        if (self->m_isActive)
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
                self->m_isActive = false;
            }
        }
        else
        {
            if (input->validatePressed)
            {
                self->m_isActive = true;
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
    UISystem_playSFX(uiSystem, selfSelectable->m_audioOnClick);
}
