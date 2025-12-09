/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_button.h"
#include "game_engine_common.h"

UIButton* UIButton_create(const char* objectName, TTF_Font* font)
{
    UIButton* self = (UIButton*)calloc(1, sizeof(UIButton));
    AssertNew(self);

    UIButton_init(self, objectName, font);

    return self;
}

void UIButton_init(void* self, const char* objectName, TTF_Font* font)
{
    assert(self && "self must not be NULL");

    UISelectable_init(self, objectName);
    UIObject* selfObj = (UIObject*)self;
    UISelectable* selfSelectable = (UISelectable*)self;
    UIButton* selfButton = (UIButton*)self;

    const char* defaultText = "Text Button";

    selfObj->m_type |= UI_TYPE_BUTTON;
    selfButton->m_labelAnchor = Vec2_set(0.5f, 0.5f);
    selfButton->m_useColorMod = false;
    selfButton->m_labelText = TTF_CreateText(g_textEngine, font, defaultText, strlen(defaultText));
    AssertNew(selfButton->m_labelText);

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
    for (int i = 0; i < UI_BUTTON_STATE_COUNT; ++i)
    {
        selfButton->m_labelColors[i] = defaultTextColors[i];
        selfButton->m_backColors[i] = defaultBackColors[i];
        selfButton->m_spriteIndices[i] = -1;
    }

    // Virtual methods
    selfObj->m_onDestroy = UIButtonVM_onDestroy;
    selfObj->m_onRender = UIButtonVM_onRender;
    selfObj->m_onUpdate = UIButtonVM_onUpdate;
    selfSelectable->m_onFocusChanged = UIButtonVM_onFocusChanged;
    selfSelectable->m_onFocus = UIButtonVM_onFocus;
    selfButton->m_onClick = UIButtonVM_onClick;
}

void UIButtonVM_onDestroy(void* self)
{
    UIButton* selfButton = (UIButton*)self;
    SDL_free(selfButton->m_labelString);
    TTF_DestroyText(selfButton->m_labelText);

    UISelectableVM_onDestroy(self);
}

static void UIButton_updateButtonState(void* self)
{
    UIFocusState selectableState = ((UISelectable*)self)->m_focusState;
    UIButton* selfButton = (UIButton*)self;
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

void UIButtonVM_onUpdate(void* self)
{
    UISelectableVM_onUpdate(self);
    UIButton_updateButtonState(self);

    UIObject* selfObj = (UIObject*)self;
    UIButton* selfButton = (UIButton*)self;

    UITransform_updateAABB(
        &selfButton->m_symbolTransform,
        &selfObj->m_transform
    );
}

void UIButtonVM_onRender(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UISelectable* selfSelectable = (UISelectable*)self;
    UIButton* selfButton = (UIButton*)self;
    bool success = true;

    UIButtonState state = selfButton->m_buttonState;
    TTF_Text* ttfText = selfButton->m_labelText;

    // Update text
    SDL_Color textColor = selfButton->m_labelColors[state];
    success = TTF_SetTextString(ttfText, selfButton->m_labelString, strlen(selfButton->m_labelString));
    assert(success);

    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(selfObj, &viewportRect);

    UIUtils_renderSprite(
        selfButton->m_spriteGroup,
        selfButton->m_spriteIndices[state],
        selfButton->m_backColors[state],
        selfButton->m_useColorMod,
        &viewportRect
    );

    if (selfButton->m_symbolGroup && selfButton->m_symbolSpriteIndex >= 0)
    {
        SDL_FRect symbolRect = { 0 };
        UITransform_getViewportRect(&selfButton->m_symbolTransform, &symbolRect);
        UIUtils_renderSprite(
            selfButton->m_symbolGroup,
            selfButton->m_symbolSpriteIndex,
            textColor,
            selfButton->m_useColorMod,
            &symbolRect
        );
    }

    UIUtils_renderText(ttfText, &viewportRect, selfButton->m_labelAnchor, &textColor);
}

void UIButton_setLabelString(void* self, const char* text)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    assert(text && "text must not be NULL");
    UIButton* selfButton = (UIButton*)self;

    assert(selfButton->m_labelString && "Existing text string must not be NULL");
    SDL_free(selfButton->m_labelString);
    selfButton->m_labelString = SDL_strdup(text);
}

void UIButtonVM_onClick(void* self)
{
    // Nothing to do
}

void UIButtonVM_onFocus(void* self, UIInput* input)
{
    UISelectableVM_onFocus(self, input);

    UIObject* selfObj = (UIObject*)self;
    UISelectable* selfSelectable = (UISelectable*)self;
    UIButton* selfButton = (UIButton*)self;

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

void UIButtonVM_onFocusChanged(void* self, UIFocusState currState, UIFocusState prevState)
{
    UISelectableVM_onFocusChanged(self, currState, prevState);

    UIButton* selfButton = (UIButton*)self;
    if (currState == UI_FOCUS_STATE_NORMAL || currState == UI_FOCUS_STATE_DISABLED)
    {
        selfButton->m_isPressed = false;
    }
}
