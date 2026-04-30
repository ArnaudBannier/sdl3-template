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

void UIImageVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
#define UIImageVM_onUpdate UIObjectVM_onUpdate
#define UIImageVM_onDestroy UIObjectVM_onDestroy
#define UIImageVM_onDrawGizmos UIObjectVM_onDrawGizmos

typedef struct UIImage
{
    UIObject m_baseObject;
    SpriteGroup* m_spriteGroup;
    int m_spriteIndex;
    RenderBlendMod m_blendMod;
    RenderColorMod m_colorMod;
} UIImage;

UIImage* UIImage_create(UISystem* uiSystem, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex);
void UIImage_init(void* selfPtr, UISystem* uiSystem, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex);
INLINE void UIImage_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UIImage_setColorMod(void* selfPtr, SDL_Color colorMod)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)selfPtr;
    RenderColorMod_setColor(&(selfImage->m_colorMod), colorMod);
}

INLINE void UIImage_setOpacity(void* selfPtr, float opacity)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)selfPtr;
    selfImage->m_blendMod.alpha = opacity;
}

INLINE void UIImage_setSpriteIndex(void* selfPtr, int spriteIndex)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)selfPtr;
    selfImage->m_spriteIndex = spriteIndex;
}

INLINE void UIImage_setSprite(void* selfPtr, SpriteGroup* spriteGroup, int spriteIndex)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_IMAGE) && "self must be of type UI_TYPE_IMAGE");
    UIImage* selfImage = (UIImage*)selfPtr;
    selfImage->m_spriteGroup = spriteGroup;
    selfImage->m_spriteIndex = spriteIndex;
}
