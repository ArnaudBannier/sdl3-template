/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_selectable.h"
#include "core/sprite_sheet.h"

void UIButtonVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
void UIButtonVM_onDestroy(void* selfPtr);
void UIButtonVM_onUpdate(void* selfPtr);
void UIButtonVM_onClick(void* selfPtr);
void UIButtonVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState);
void UIButtonVM_onFocus(void* selfPtr, UIInput* input);
#define UIButtonVM_onDrawGizmos UISelectableVM_onDrawGizmos

typedef enum UIButtonState
{
    UI_BUTTON_STATE_NORMAL = 0,
    UI_BUTTON_STATE_FOCUSED,
    UI_BUTTON_STATE_PRESSED,
    UI_BUTTON_STATE_ACTIVE,
    UI_BUTTON_STATE_ACTIVE_FOCUSED,
    UI_BUTTON_STATE_ACTIVE_PRESSED,
    UI_BUTTON_STATE_DISABLED,
    //
    UI_BUTTON_STATE_COUNT
} UIButtonState;

typedef struct UIButton
{
    UISelectable baseSelectable;
    TTF_Text* labelText;

    char* labelString;
    SDL_Color labelColors[UI_BUTTON_STATE_COUNT];
    SDL_Color backColors[UI_BUTTON_STATE_COUNT];

    SpriteGroup* spriteGroup;
    int spriteIndices[UI_BUTTON_STATE_COUNT];

    SpriteGroup* symbolGroup;
    int symbolSpriteIndex;
    UITransform symbolTransform;
    UITransform labelTransform;

    Vec2 labelAnchor;
    bool isActive;
    bool isPressed;
    bool useColorMod;
    UIButtonState buttonState;

    void (*onClick)(void* selfPtr);
    void (*userOnClick)(void* selfPtr);
} UIButton;

UIButton* UIButton_create(UISystem* uiSystem, const char* objectName, TTF_Font* font);
void UIButton_init(void* selfPtr, UISystem* uiSystem, const char* objectName, TTF_Font* font);
INLINE void UIButton_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UIButton_setLabelColor(void* selfPtr, UIButtonState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->labelColors[state] = color;
}

INLINE void UIButton_setBackgroundColor(void* selfPtr, UIButtonState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->backColors[state] = color;
}

INLINE void UIButton_setLabelAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->labelAnchor = anchor;
}

INLINE void UIButton_setSpriteGroup(void* selfPtr, SpriteGroup* spriteGroup)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->spriteGroup = spriteGroup;
}

INLINE void UIButton_setSpriteIndex(void* selfPtr, UIButtonState state, int spriteIndex)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->spriteIndices[state] = spriteIndex;
}

INLINE void UIButton_setUseColorMod(void* selfPtr, bool useColorMod)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->useColorMod = useColorMod;
}

INLINE void UIButton_setActiveState(void* selfPtr, bool isActive)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->isActive = isActive;
}

INLINE bool UIButton_isActive(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    return selfButton->isActive;
}

INLINE void UIButton_setSymbolSprite(
    void* selfPtr,
    SpriteGroup* symbolGroup,
    int spriteIndex,
    UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->symbolGroup = symbolGroup;
    selfButton->symbolSpriteIndex = spriteIndex;
    selfButton->symbolTransform.localRect = rect;
}

void UIButton_setLabelString(void* selfPtr, const char* text);

INLINE void UIButton_setLabelRect(void* selfPtr, UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->labelTransform.localRect = rect;
}

INLINE void UIButton_setFont(void* selfPtr, TTF_Font* font)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    if (selfButton->labelText)
    {
        TTF_SetTextFont(selfButton->labelText, font);
    }
}

INLINE void UIButton_setOnClickCallback(
    void* selfPtr,
    void (*onClick)(void* selfPtr))
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->userOnClick = onClick;
}
