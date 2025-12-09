/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"

void UIFillRectVM_onRender(void* self);
#define UIFillRectVM_onUpdate UIObjectVM_onUpdate
#define UIFillRectVM_onDestroy UIObjectVM_onDestroy

typedef struct UIFillRect
{
    UIObject m_baseObject;
    SpriteGroup* m_spriteGroup;
    SDL_Color m_color;
    float m_opacity;
} UIFillRect;

UIFillRect* UIFillRect_create(const char* objectName, SDL_Color color);
void UIFillRect_init(void* self, const char* objectName, SDL_Color color);
INLINE void UIFillRect_destroy(void* self)
{
    UIObject_destroy(self);
}
