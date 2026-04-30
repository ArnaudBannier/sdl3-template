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
    UISelectable m_baseSelectable;
    TTF_Text* m_labelText;

    char* m_labelString;
    SDL_Color m_labelColors[UI_BUTTON_STATE_COUNT];
    SDL_Color m_backColors[UI_BUTTON_STATE_COUNT];

    SpriteGroup* m_spriteGroup;
    int m_spriteIndices[UI_BUTTON_STATE_COUNT];

    SpriteGroup* m_symbolGroup;
    int m_symbolSpriteIndex;
    UITransform m_symbolTransform;
    UITransform m_labelTransform;

    Vec2 m_labelAnchor;
    bool m_isActive;
    bool m_isPressed;
    bool m_useColorMod;
    UIButtonState m_buttonState;

    void (*m_onClick)(void* selfPtr);
    void (*m_userOnClick)(void* selfPtr);
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
    selfButton->m_labelColors[state] = color;
}

INLINE void UIButton_setBackgroundColor(void* selfPtr, UIButtonState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_backColors[state] = color;
}

INLINE void UIButton_setLabelAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_labelAnchor = anchor;
}

INLINE void UIButton_setSpriteGroup(void* selfPtr, SpriteGroup* spriteGroup)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_spriteGroup = spriteGroup;
}

INLINE void UIButton_setSpriteIndex(void* selfPtr, UIButtonState state, int spriteIndex)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_spriteIndices[state] = spriteIndex;
}

INLINE void UIButton_setUseColorMod(void* selfPtr, bool useColorMod)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_useColorMod = useColorMod;
}

INLINE void UIButton_setActiveState(void* selfPtr, bool isActive)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_isActive = isActive;
}

INLINE bool UIButton_isActive(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    return selfButton->m_isActive;
}

INLINE void UIButton_setSymbolSprite(
    void* selfPtr,
    SpriteGroup* symbolGroup,
    int spriteIndex,
    UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_symbolGroup = symbolGroup;
    selfButton->m_symbolSpriteIndex = spriteIndex;
    selfButton->m_symbolTransform.localRect = rect;
}

void UIButton_setLabelString(void* selfPtr, const char* text);

INLINE void UIButton_setLabelRect(void* selfPtr, UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_labelTransform.localRect = rect;
}

INLINE void UIButton_setFont(void* selfPtr, TTF_Font* font)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    if (selfButton->m_labelText)
    {
        TTF_SetTextFont(selfButton->m_labelText, font);
    }
}

INLINE void UIButton_setOnClickCallback(
    void* selfPtr,
    void (*onClick)(void* selfPtr))
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)selfPtr;
    selfButton->m_userOnClick = onClick;
}
