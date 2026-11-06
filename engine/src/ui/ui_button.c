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

    selfObj->type |= UI_TYPE_BUTTON;
    selfButton->labelAnchor = Vec2_set(0.5f, 0.5f);
    selfButton->useColorMod = false;
    selfButton->labelText = TTF_CreateText(g_engine.sdl.textEngine, font, defaultText, strlen(defaultText));
    ASSERT_NEW(selfButton->labelText);

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
    selfButton->labelString = SDL_strdup(defaultText);
    for (int i = 0; i < UI_BUTTON_STATE_COUNT; i++)
    {
        selfButton->labelColors[i] = defaultTextColors[i];
        selfButton->backColors[i] = defaultBackColors[i];
        selfButton->spriteIndices[i] = -1;
    }

    // Virtual methods
    selfObj->onDestroy = UIButtonVM_onDestroy;
    selfObj->onRender = UIButtonVM_onRender;
    selfObj->onUpdate = UIButtonVM_onUpdate;
    selfObj->onDrawGizmos = UIButtonVM_onDrawGizmos;
    selfSelectable->onFocus = UIButtonVM_onFocus;
    selfSelectable->onFocusChanged = UIButtonVM_onFocusChanged;
    selfButton->onClick = UIButtonVM_onClick;
}

void UIButtonVM_onDestroy(void* selfPtr)
{
    UIButton* selfButton = (UIButton*)selfPtr;
    SDL_free(selfButton->labelString);
    TTF_DestroyText(selfButton->labelText);

    UISelectableVM_onDestroy(selfPtr);
}

static void UIButton_updateButtonState(void* selfPtr)
{
    UIFocusState selectableState = ((UISelectable*)selfPtr)->focusState;
    UIButton* selfButton = (UIButton*)selfPtr;
    if (selectableState == UI_FOCUS_STATE_DISABLED)
    {
        selfButton->buttonState = UI_BUTTON_STATE_DISABLED;
        return;
    }

    if (selfButton->isActive)
    {
        if (selfButton->isPressed)
        {
            selfButton->buttonState = UI_BUTTON_STATE_ACTIVE_PRESSED;
            return;
        }

        switch (selectableState)
        {
        default:
        case UI_FOCUS_STATE_NORMAL:
            selfButton->buttonState = UI_BUTTON_STATE_ACTIVE;
            return;

        case UI_FOCUS_STATE_FOCUSED:
            selfButton->buttonState = UI_BUTTON_STATE_ACTIVE_FOCUSED;
            return;
        }
    }
    else
    {
        if (selfButton->isPressed)
        {
            selfButton->buttonState = UI_BUTTON_STATE_PRESSED;
            return;
        }

        switch (selectableState)
        {
        default:
        case UI_FOCUS_STATE_NORMAL:
            selfButton->buttonState = UI_BUTTON_STATE_NORMAL;
            return;

        case UI_FOCUS_STATE_FOCUSED:
            selfButton->buttonState = UI_BUTTON_STATE_FOCUSED;
            return;
        }
    }
}

void UIButtonVM_onUpdate(void* selfPtr)
{
    UISelectableVM_onUpdate(selfPtr);
    UIObject* selfObj = (UIObject*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;
    UIButtonState prevState = selfButton->buttonState;
    UIButton_updateButtonState(selfPtr);
    UIButtonState currState = selfButton->buttonState;

    if (prevState != currState)
    {
        if (currState == UI_BUTTON_STATE_ACTIVE_PRESSED || currState == UI_BUTTON_STATE_PRESSED)
        {
            UISystem* uiSystem = UIObject_getUISystem(selfPtr);
            UISelectable* selfSelectable = (UISelectable*)selfPtr;
            UISystem_playSFX(uiSystem, selfSelectable->audioOnPressed);
        }
    }

    UITransform_updateAABB(
        &selfButton->symbolTransform,
        &selfObj->transform
    );

    // Update text
    TTF_Text* ttfText = selfButton->labelText;
    bool success = TTF_SetTextString(ttfText, selfButton->labelString, strlen(selfButton->labelString));
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_SYSTEM);
}

void UIButtonVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;
    bool success = true;

    UIButtonState state = selfButton->buttonState;
    UITransform* uiTransform = &selfObj->transform;
    SDL_Color textColor = selfButton->labelColors[state];
    SDL_Color backColor = selfButton->backColors[state];

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
    if (selfButton->spriteGroup && selfButton->spriteIndices[state] >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfButton->spriteGroup,
            selfButton->spriteIndices[state]
        );
        cmd.texture = &texture;
    }
    else
    {
        rect.color = backColor;
        rect.filled = true;
        cmd.rect = &rect;
    }

    if (selfButton->useColorMod)
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
        .ttfText = selfButton->labelText,
        .color = textColor,
        .anchor = selfButton->labelAnchor
    };
    cmd.text = &renderText;
    cmd.colorMod = NULL;
    cmd.rect = NULL;
    cmd.texture = NULL;

    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // Symbol
    if (selfButton->symbolGroup && selfButton->symbolSpriteIndex >= 0)
    {
        RenderTexture_setFromGroup(
            &texture,
            selfButton->symbolGroup,
            selfButton->symbolSpriteIndex
        );
        cmd.texture = &texture;
        cmd.rect = NULL;
        cmd.text = NULL;

        UITransform_getComponents(
            &(selfButton->symbolTransform),
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

    assert(selfButton->labelString && "Existing text string must not be NULL");
    SDL_free(selfButton->labelString);
    selfButton->labelString = SDL_strdup(text);
}

void UIButtonVM_onClick(void* selfPtr)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UISystem* uiSystem = UIObject_getUISystem(selfPtr);
    UISystem_playSFX(uiSystem, selfSelectable->audioOnClick);
}

void UIButtonVM_onFocus(void* selfPtr, UIInput* input)
{
    UISelectableVM_onFocus(selfPtr, input);

    UIObject* selfObj = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIButton* selfButton = (UIButton*)selfPtr;

    bool wasPressed = selfButton->isPressed;

    if (input->lastInputType == UI_INPUT_TYPE_MOUSE)
    {
        if (UITransform_containsPoint(&selfObj->transform, input->mouseUIPos))
        {
            if (selfButton->isPressed)
            {
                selfButton->isPressed = input->clickDown;
            }
            else
            {
                selfButton->isPressed = input->clickPressed;
            }

            if (wasPressed && input->clickReleased)
            {
                selfButton->onClick(selfButton);
                if (selfButton->userOnClick)
                {
                    selfButton->userOnClick(selfButton);
                }
            }
        }
        else
        {
            selfButton->isPressed = false;
        }
    }
    else
    {
        if (selfButton->isPressed)
        {
            selfButton->isPressed = input->validateDown;
        }
        else
        {
            selfButton->isPressed = input->validatePressed;
        }

        if (wasPressed && input->validateReleased)
        {
            selfButton->onClick(selfButton);
            if (selfButton->userOnClick)
            {
                selfButton->userOnClick(selfButton);
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
        selfButton->isPressed = false;
    }
}
