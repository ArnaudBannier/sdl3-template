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

    selfObject->type |= UI_TYPE_IMAGE;
    selfObject->gizmosColor = g_colors.teal5;
    selfObject->gizmosIsFilled = false;
    selfObject->gizmosOpacity = 1.f;
    selfImage->spriteGroup = spriteGroup;
    selfImage->spriteIndex = spriteIndex;

    RenderColorMod_init(&(selfImage->colorMod));
    RenderBlendMod_init(&(selfImage->blendMod));

    // Virtual methods
    selfObject->onRender = UIImageVM_onRender;
    selfObject->onUpdate = UIImageVM_onUpdate;
    selfObject->onDestroy = UIImageVM_onDestroy;
    selfObject->onDrawGizmos = UIImageVM_onDrawGizmos;
}

void UIImageVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIImage* selfImage = (UIImage*)selfPtr;

    Transform transform = { 0 };
    RenderAnchor anchor = { 0 };
    RenderDim dim = { 0 };
    RenderTexture texture = { 0 };

    UITransform_getComponents(&(selfObj->transform), &transform, &dim, &anchor);
    RenderTexture_setFromGroup(&texture, selfImage->spriteGroup, selfImage->spriteIndex);

    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->layer);
    cmd.anchor = &anchor;
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.texture = &texture;
    cmd.blendMod = &(selfImage->blendMod);
    cmd.colorMod = &(selfImage->colorMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}
