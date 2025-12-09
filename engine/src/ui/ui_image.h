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

void UIImageVM_onRender(void* self);
#define UIImageVM_onUpdate UIObjectVM_onUpdate
#define UIImageVM_onDestroy UIObjectVM_onDestroy

typedef struct UIImage
{
    UIObject m_baseObject;
    SpriteGroup* m_spriteGroup;
    int m_spriteIndex;
    SDL_Color m_colorMod;
    float m_opacity;
} UIImage;

UIImage* UIImage_create(const char* objectName, SpriteGroup* spriteGroup, int spriteIndex);
void UIImage_init(void* self, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex);
INLINE void UIImage_destroy(void* self)
{
    UIObject_destroy(self);
}

INLINE void UIImage_setColorMod(void* self, SDL_Color colorMod)
{
    assert(UIObject_isOfType(self, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)self;
    selfImage->m_colorMod = colorMod;
}

INLINE void UIImage_setOpacity(void* self, float opacity)
{
    assert(UIObject_isOfType(self, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)self;
    selfImage->m_opacity = opacity;
}

INLINE void UIImage_setSpriteIndex(void* self, int spriteIndex)
{
    assert(UIObject_isOfType(self, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)self;
    selfImage->m_spriteIndex = spriteIndex;
}

INLINE void UIImage_setSprite(void* self, SpriteGroup* spriteGroup, int spriteIndex)
{
    assert(UIObject_isOfType(self, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)self;
    selfImage->m_spriteGroup = spriteGroup;
    selfImage->m_spriteIndex = spriteIndex;
}
