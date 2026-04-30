/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_image.h"
#include "engine_common.h"

UIImage* UIImage_create(UISystem* uiSystem, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex)
{
    UIImage* self = (UIImage*)calloc(1, sizeof(UIImage));
    ASSERT_NEW(self);

    UIImage_init(self, uiSystem, objectName, spriteGroup, spriteIndex);

    return self;
}

void UIImage_init(void* selfPtr, UISystem* uiSystem, const char* objectName, SpriteGroup* spriteGroup, int spriteIndex)
{
    assert(selfPtr && "self must not be NULL");

    UIObject* selfObject = (UIObject*)selfPtr;
    UIImage* selfImage = (UIImage*)selfPtr;
    UIObject_init(selfObject, uiSystem, objectName);

    selfObject->m_type |= UI_TYPE_IMAGE;
    selfObject->m_gizmosColor = g_colors.teal5;
    selfObject->m_gizmosIsFilled = false;
    selfObject->m_gizmosOpacity = 1.f;
    selfImage->m_spriteGroup = spriteGroup;
    selfImage->m_spriteIndex = spriteIndex;

    RenderColorMod_init(&(selfImage->m_colorMod));
    RenderBlendMod_init(&(selfImage->m_blendMod));

    // Virtual methods
    selfObject->m_onRender = UIImageVM_onRender;
    selfObject->m_onUpdate = UIImageVM_onUpdate;
    selfObject->m_onDestroy = UIImageVM_onDestroy;
    selfObject->m_onDrawGizmos = UIImageVM_onDrawGizmos;
}

void UIImageVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIImage* selfImage = (UIImage*)selfPtr;

    Transform transform = { 0 };
    RenderAnchor anchor = { 0 };
    RenderDim dim = { 0 };
    RenderTexture texture = { 0 };

    UITransform_getComponents(&(selfObj->m_transform), &transform, &dim, &anchor);
    RenderTexture_setFromGroup(&texture, selfImage->m_spriteGroup, selfImage->m_spriteIndex);

    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->m_layer);
    cmd.anchor = &anchor;
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.texture = &texture;
    cmd.blendMod = &(selfImage->m_blendMod);
    cmd.colorMod = &(selfImage->m_colorMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}
