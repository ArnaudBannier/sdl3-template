/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "utils/utils.h"

void UILabelVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
void UILabelVM_onDestroy(void* selfPtr);
#define UILabelVM_onUpdate UIObjectVM_onUpdate
#define UILabelVM_onDrawGizmos UIObjectVM_onDrawGizmos

typedef struct UILabel
{
    UIObject m_baseObject;
    char* m_textString;
    RenderText m_text;
    RenderBlendMod m_blendMod;
} UILabel;

UILabel* UILabel_create(UISystem* uiSystem, const char* objectName, TTF_Font* font);
void UILabel_init(void* selfPtr, UISystem* uiSystem, const char* objectName, TTF_Font* font);
INLINE void UILabel_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UILabel_setColor(void* selfPtr, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)selfPtr;
    selfLabel->m_text.color = color;
}

INLINE void UILabel_setAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)selfPtr;
    selfLabel->m_text.anchor = anchor;
}

INLINE void UILabel_setTextString(void* selfPtr, const char* text)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)selfPtr;
    assert(selfLabel->m_textString && "Existing ttfText string must not be NULL");
    SDL_free(selfLabel->m_textString);
    selfLabel->m_textString = SDL_strdup(text);
}

INLINE void UILabel_setOpacity(void* selfPtr, float opacity)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LABEL) && "self must be of type UI_TYPE_LABEL");
    UILabel* selfLabel = (UILabel*)selfPtr;
    selfLabel->m_blendMod.alpha = opacity;
}
