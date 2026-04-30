/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"

void UIFillRectVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
#define UIFillRectVM_onUpdate UIObjectVM_onUpdate
#define UIFillRectVM_onDestroy UIObjectVM_onDestroy
#define UIFillRectVM_onDrawGizmos UIObjectVM_onDrawGizmos

typedef struct UIFillRect
{
    UIObject m_baseObject;
    RenderRect m_rect;
    RenderBlendMod m_blendMod;
} UIFillRect;

UIFillRect* UIFillRect_create(UISystem* uiSystem, const char* objectName, SDL_Color color);
void UIFillRect_init(void* selfPtr, UISystem* uiSystem, const char* objectName, SDL_Color color);
INLINE void UIFillRect_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UIFillRect_setColor(void* selfPtr, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_FILL_RECT) && "self must be of type UI_TYPE_FILL_RECT");
    UIFillRect* selfFillRect = (UIFillRect*)selfPtr;
    selfFillRect->m_rect.color = color;
}

INLINE void UIFillRect_setOpacity(void* selfPtr, float opacity)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_FILL_RECT) && "self must be of type UI_TYPE_FILL_RECT");
    UIFillRect* selfFillRect = (UIFillRect*)selfPtr;
    selfFillRect->m_blendMod.alpha = opacity;
}
