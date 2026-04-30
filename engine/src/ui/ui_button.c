/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_button.h"
#include "ui/ui_system.h"
#include "engine_common.h"

UIButton* UIButton_create(UISystem* uiSystem, const char* objectName, TTF_Font* font)
{
    UIButton* self = (UIButton*)calloc(1, sizeof(UIButton));
    ASSERT_NEW(self);

    UIButton_init(self, uiSystem, objectName, font);

    return self;
}

void UIButton_init(void* selfPtr, UISystem* uiSystem, const char* objectName, TTF_Font* font)
{
    assert(selfPtr && "self must not be NULL");

    UISelectable_init(selfPtr, uiSystem, objectName);
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;

    const char* defaultText = "Text Button";

    selfObj->m_type |= UI_TYPE_BUTTON;
    selfButton->m_labelAnchor = Vec2_set(0.5f, 0.5f);
    selfButton->m_useColorMod = false;
    selfButton->m_labelText = TTF_CreateText(g_engine.sdl.textEngine, font, defaultText, strlen(defaultText));
    ASSERT_NEW(selfButton->m_labelText);

    SDL_Color defaultTextColors[UI_BUTTON_STATE_COUNT] = { 0 };
    SDL_Color defaultBackColors[UI_BUTTON_STATE_COUNT] = { 0 };

    defaultTextColors[UI_BUTTON_STATE_NORMAL] = (SDL_Color){ 200, 200, 200, 255 };
    defaultTextColors[UI_BUTTON_STATE_FOCUSED] = (SDL_Color){ 255, 255, 255, 255 };
    defaultTextColors[UI_BUTTON_STATE_PRESSED] = (SDL_Color){ 128, 128, 128, 255 };
    defaultTextColors[UI_BUTTON_STATE_ACTIVE] = (SDL_Color){ 200, 200, 200, 255 };
    defaultTextColors[UI_BUTTON_STATE_ACTIVE_FOCUSED] = (SDL_Color){ 255, 255, 255, 255 };
    defaultTextColors[UI_BUTTON_STATE_ACTIVE_PRESSED] = (SDL_Color){ 128, 128, 128, 255 };
    defaultTextColors[UI_BUTTON_STATE_DISABLED] = (SDL_Color){ 40, 40, 40, 255 };

    defaultBackColors[UI_BUTTON_STATE_NORMAL] = (SDL_Color){ 50, 50, 50, 255 };
    defaultBackColors[UI_BUTTON_STATE_FOCUSED] = (SDL_Color){ 70, 70, 70, 255 };
    defaultBackColors[UI_BUTTON_STATE_PRESSED] = (SDL_Color){ 25, 25, 25, 255 };
    defaultBackColors[UI_BUTTON_STATE_ACTIVE] = (SDL_Color){ 100, 100, 100, 255 };
    defaultBackColors[UI_BUTTON_STATE_ACTIVE_FOCUSED] = (SDL_Color){ 120, 120, 120, 255 };
    defaultBackColors[UI_BUTTON_STATE_ACTIVE_PRESSED] = (SDL_Color){ 80, 80, 80, 255 };
    defaultBackColors[UI_BUTTON_STATE_DISABLED] = (SDL_Color){ 20, 20, 20, 255 };

    // Initialize text contents and colors
    selfButton->m_labelString = SDL_strdup(defaultText);
    for (int i = 0; i < UI_BUTTON_STATE_COUNT; i++)
    {
        selfButton->m_labelColors[i] = defaultTextColors[i];
        selfButton->m_backColors[i] = defaultBackColors[i];
        selfButton->m_spriteIndices[i] = -1;
    }

    // Virtual methods
    selfObj->m_onDestroy = UIButtonVM_onDestroy;
    selfObj->m_onRender = UIButtonVM_onRender;
    selfObj->m_onUpdate = UIButtonVM_onUpdate;
    selfObj->m_onDrawGizmos = UIButtonVM_onDrawGizmos;
    selfSelectable->m_onFocus = UIButtonVM_onFocus;
    selfSelectable->m_onFocusChanged = UIButtonVM_onFocusChanged;
    selfButton->m_onClick = UIButtonVM_onClick;
}

void UIButtonVM_onDestroy(void* selfPtr)
{
    UIButton* selfButton = (UIButton*)selfPtr;
    SDL_free(selfButton->m_labelString);
    TTF_DestroyText(selfButton->m_labelText);

    UISelectableVM_onDestroy(selfPtr);
}

static void UIButton_updateButtonState(void* selfPtr)
{
    UIFocusState selectableState = ((UISelectable*)selfPtr)->m_focusState;
    UIButton* selfButton = (UIButton*)selfPtr;
    if (selectableState == UI_FOCUS_STATE_DISABLED)
    {
        selfButton->m_buttonState = UI_BUTTON_STATE_DISABLED;
        return;
    }

    if (selfButton->m_isActive)
    {
        if (selfButton->m_isPressed)
        {
            selfButton->m_buttonState = UI_BUTTON_STATE_ACTIVE_PRESSED;
            return;
        }

        switch (selectableState)
        {
        default:
        case UI_FOCUS_STATE_NORMAL:
            selfButton->m_buttonState = UI_BUTTON_STATE_ACTIVE;
            return;

        case UI_FOCUS_STATE_FOCUSED:
            selfButton->m_buttonState = UI_BUTTON_STATE_ACTIVE_FOCUSED;
            return;
        }
    }
    else
    {
        if (selfButton->m_isPressed)
        {
            selfButton->m_buttonState = UI_BUTTON_STATE_PRESSED;
            return;
        }

        switch (selectableState)
        {
        default:
        case UI_FOCUS_STATE_NORMAL:
            selfButton->m_buttonState = UI_BUTTON_STATE_NORMAL;
            return;

        case UI_FOCUS_STATE_FOCUSED:
            selfButton->m_buttonState = UI_BUTTON_STATE_FOCUSED;
            return;
        }
    }
}

void UIButtonVM_onUpdate(void* selfPtr)
{
    UISelectableVM_onUpdate(selfPtr);
    UIObject* selfObj = (UIObject*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;
    UIButtonState prevState = selfButton->m_buttonState;
    UIButton_updateButtonState(selfPtr);
    UIButtonState currState = selfButton->m_buttonState;

    if (prevState != currState)
    {
        if (currState == UI_BUTTON_STATE_ACTIVE_PRESSED || currState == UI_BUTTON_STATE_PRESSED)
        {
            UISystem* uiSystem = UIObject_getUISystem(selfPtr);
            UISelectable* selfSelectable = (UISelectable*)selfPtr;
            UISystem_playSFX(uiSystem, selfSelectable->m_audioOnPressed);
        }
    }

    UITransform_updateAABB(
        &selfButton->m_symbolTransform,
        &selfObj->m_transform
    );

    // Update text
    TTF_Text* ttfText = selfButton->m_labelText;
    bool success = TTF_SetTextString(ttfText, selfButton->m_labelString, strlen(selfButton->m_labelString));
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_SYSTEM);
}

void UIButtonVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;
    bool success = true;

    UIButtonState state = selfButton->m_buttonState;
    UITransform* uiTransform = &selfObj->m_transform;
    SDL_Color textColor = selfButton->m_labelColors[state];
    SDL_Color backColor = selfButton->m_backColors[state];

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
    if (selfButton->m_spriteGroup && selfButton->m_spriteIndices[state] >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfButton->m_spriteGroup,
            selfButton->m_spriteIndices[state]
        );
        cmd.texture = &texture;
    }
    else
    {
        rect.color = backColor;
        rect.filled = true;
        cmd.rect = &rect;
    }

    if (selfButton->m_useColorMod)
    {
        colorMod.r = backColor.r / 255.f;
        colorMod.g = backColor.g / 255.f;
        colorMod.b = backColor.b / 255.f;
        cmd.colorMod = &colorMod;
    }
    UITransform_getComponents(uiTransform, &transform, &dim, &anchor);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);


    // Text
    RenderText renderText = {
        .ttfText = selfButton->m_labelText,
        .color = textColor,
        .anchor = selfButton->m_labelAnchor
    };
    cmd.text = &renderText;
    cmd.colorMod = NULL;
    cmd.rect = NULL;
    cmd.texture = NULL;

    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Symbol
    if (selfButton->m_symbolGroup && selfButton->m_symbolSpriteIndex >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfButton->m_symbolGroup,
            selfButton->m_symbolSpriteIndex
        );
        cmd.texture = &texture;
        cmd.rect = NULL;
        cmd.text = NULL;

        UITransform_getComponents(
            &(selfButton->m_symbolTransform),
            &transform, &dim, &anchor
        );

        GraphicsSystem_addCommand(graphicsSystem, &cmd);
    }
}

void UIButton_setLabelString(void* selfPtr, const char* text)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    assert(text && "text must not be NULL");
    UIButton* selfButton = (UIButton*)selfPtr;

    assert(selfButton->m_labelString && "Existing text string must not be NULL");
    SDL_free(selfButton->m_labelString);
    selfButton->m_labelString = SDL_strdup(text);
}

void UIButtonVM_onClick(void* selfPtr)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UISystem* uiSystem = UIObject_getUISystem(selfPtr);
    UISystem_playSFX(uiSystem, selfSelectable->m_audioOnClick);
}

void UIButtonVM_onFocus(void* selfPtr, UIInput* input)
{
    UISelectableVM_onFocus(selfPtr, input);

    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;

    bool wasPressed = selfButton->m_isPressed;

    if (input->lastInputType == UI_INPUT_TYPE_MOUSE)
    {
        if (UITransform_containsPoint(&selfObj->m_transform, input->mouseUIPos))
        {
            if (selfButton->m_isPressed)
            {
                selfButton->m_isPressed = input->clickDown;
            }
            else
            {
                selfButton->m_isPressed = input->clickPressed;
            }

            if (wasPressed && input->clickReleased)
            {
                selfButton->m_onClick(selfButton);
                if (selfButton->m_userOnClick)
                {
                    selfButton->m_userOnClick(selfButton);
                }
            }
        }
        else
        {
            selfButton->m_isPressed = false;
        }
    }
    else
    {
        if (selfButton->m_isPressed)
        {
            selfButton->m_isPressed = input->validateDown;
        }
        else
        {
            selfButton->m_isPressed = input->validatePressed;
        }

        if (wasPressed && input->validateReleased)
        {
            selfButton->m_onClick(selfButton);
            if (selfButton->m_userOnClick)
            {
                selfButton->m_userOnClick(selfButton);
            }
        }
    }
}

void UIButtonVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState)
{
    UISelectableVM_onFocusChanged(selfPtr, currState, prevState);

    UIButton* selfButton = (UIButton*)selfPtr;
    if (currState == UI_FOCUS_STATE_NORMAL || currState == UI_FOCUS_STATE_DISABLED)
    {
        selfButton->m_isPressed = false;
    }
}
