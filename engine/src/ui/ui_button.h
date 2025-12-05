/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_selectable.h"
#include "core/sprite_sheet.h"

void UIButtonVM_onRender(void* self);
void UIButtonVM_onDestroy(void* self);
void UIButtonVM_onUpdate(void* self);
void UIButtonVM_onClick(void* self);
void UIButtonVM_onFocusChanged(void* self, UIFocusState currState, UIFocusState prevState);
void UIButtonVM_onFocus(void* self, UIInput* input);

typedef enum UIButtonState
{
    UI_BUTTON_STATE_NORMAL = 0,
    UI_BUTTON_STATE_FOCUSED,
    UI_BUTTON_STATE_PRESSED,
    UI_BUTTON_STATE_ACTIVE,
    UI_BUTTON_STATE_ACTIVE_FOCUSED,
    UI_BUTTON_STATE_ACTIVE_PRESSED,
    UI_BUTTON_STATE_DISABLED,
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

    void (*m_onClick)(void* self);
    void (*m_userOnClick)(void* self);
} UIButton;

UIButton* UIButton_create(const char* objectName, TTF_Font* font);
void UIButton_init(void* self, const char* objectName, TTF_Font* font);
INLINE void UIButton_destroy(void* self)
{
    UIObject_destroy(self);
}

INLINE void UIButton_setLabelColor(void* self, UIButtonState state, SDL_Color color)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_labelColors[state] = color;
}

INLINE void UIButton_setBackgroundColor(void* self, UIButtonState state, SDL_Color color)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_backColors[state] = color;
}

INLINE void UIButton_setLabelAnchor(void* self, Vec2 anchor)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_labelAnchor = anchor;
}

INLINE void UIButton_setSpriteGroup(void* self, SpriteGroup* spriteGroup)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_spriteGroup = spriteGroup;
}

INLINE void UIButton_setSpriteIndex(void* self, UIButtonState state, int spriteIndex)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_spriteIndices[state] = spriteIndex;
}

INLINE void UIButton_setUseColorMod(void* self, bool useColorMod)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_useColorMod = useColorMod;
}

INLINE void UIButton_setActiveState(void* self, bool isActive)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_isActive = isActive;
}

INLINE bool UIButton_isActive(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    return selfButton->m_isActive;
}

INLINE void UIButton_setSymbolSprite(
    void* self,
    SpriteGroup* symbolGroup,
    int spriteIndex,
    UIRect rect)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_symbolGroup = symbolGroup;
    selfButton->m_symbolSpriteIndex = spriteIndex;
    selfButton->m_symbolTransform.localRect = rect;
}

void UIButton_setLabelString(void* self, const char* text);

INLINE void UIButton_setLabelRect(void* self, UIRect rect)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_labelTransform.localRect = rect;
}

INLINE void UIButton_setFont(void* self, TTF_Font* font)
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    if (selfButton->m_labelText)
    {
        TTF_SetTextFont(selfButton->m_labelText, font);
    }
}

INLINE void UIButton_setOnClickCallback(
    void* self,
    void (*onClick)(void* self))
{
    assert(UIObject_isOfType(self, UI_TYPE_BUTTON) && "self must be of type UI_TYPE_BUTTON");
    UIButton* selfButton = (UIButton*)self;
    selfButton->m_onClick = onClick;
}
