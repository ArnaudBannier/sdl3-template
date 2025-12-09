/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_image.h"
#include "game_engine_common.h"

UIImage* UIImage_create(const char* objectName, SpriteGroup* spriteGroup, int spriteIndex)
{
    UIImage* self = (UIImage*)calloc(1, sizeof(UIImage));
    AssertNew(self);

    UIImage_init(self, objectName, spriteGroup, spriteIndex);

    return self;
}

void UIImage_init(void* self, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex)
{
    assert(self && "self must not be NULL");

    UIObject* selfObject = (UIObject*)self;
    UIImage* selfImage = (UIImage*)self;
    UIObject_init(selfObject, objectName);

    selfObject->m_type |= UI_TYPE_IMAGE;
    selfImage->m_spriteGroup = spriteGroup;
    selfImage->m_spriteIndex = spriteIndex;
    selfImage->m_colorMod = g_colors.white;
    selfImage->m_opacity = 1.f;

    // Virtual methods
    selfObject->m_onRender = UIImageVM_onRender;
    selfObject->m_onUpdate = UIImageVM_onUpdate;
    selfObject->m_onDestroy = UIImageVM_onDestroy;
}

void UIImageVM_onRender(void* self)
{
    UIImage* selfImage = (UIImage*)self;

    // Render image
    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(self, &viewportRect);
    SDL_Color colorMod = selfImage->m_colorMod;

    SpriteGroup_setOpacityFloat(selfImage->m_spriteGroup, selfImage->m_opacity);
    UIUtils_renderSprite(
        selfImage->m_spriteGroup,
        selfImage->m_spriteIndex,
        colorMod,
        true,
        &viewportRect
    );
}
