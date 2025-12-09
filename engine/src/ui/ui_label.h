/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "utils/utils.h"

void UILabelVM_onRender(void* self);
void UILabelVM_onDestroy(void* self);
#define UILabelVM_onUpdate UIObjectVM_onUpdate

typedef struct UILabel
{
    UIObject m_baseObject;
    TTF_Text* m_text;
    char* m_textString;
    SDL_Color m_color;
    Vec2 m_anchor;
} UILabel;

UILabel* UILabel_create(const char* objectName, TTF_Font* font);
void UILabel_init(void* self, const char* objectName, TTF_Font* font);
INLINE void UILabel_destroy(void* self)
{
    UIObject_destroy(self);
}

INLINE void UILabel_setColor(void* self, SDL_Color color)
{
    assert(UIObject_isOfType(self, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)self;
    selfLabel->m_color = color;
}

INLINE void UILabel_setAnchor(void* self, Vec2 anchor)
{
    assert(UIObject_isOfType(self, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)self;
    selfLabel->m_anchor = anchor;
}

INLINE void UILabel_setTextString(void* self, const char* text)
{
    assert(UIObject_isOfType(self, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)self;
    assert(selfLabel->m_textString && "Existing text string must not be NULL");
    SDL_free(selfLabel->m_textString);
    selfLabel->m_textString = SDL_strdup(text);
}
