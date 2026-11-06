/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_fill_rect.h"
#include "engine_common.h"

UIFillRect* UIFillRect_create(UISystem* uiSystem, const char* objectName, SDL_Color color)
{
    UIFillRect* self = (UIFillRect*)calloc(1, sizeof(UIFillRect));
    ASSERT_NEW(self);

    UIFillRect_init(self, uiSystem, objectName, color);

    return self;
}

void UIFillRect_init(void* selfPtr, UISystem* uiSystem, const char* objectName, SDL_Color color)
{
    assert(selfPtr && "self must not be NULL");

    UIObject* selfObject = (UIObject*)selfPtr;
    UIFillRect* selfFillRect = (UIFillRect*)selfPtr;
    UIObject_init(selfObject, uiSystem, objectName);

    selfObject->type |= UI_TYPE_FILL_RECT;
    selfFillRect->rect.filled = true;
    selfFillRect->rect.color = color;
    selfObject->gizmosColor = g_colors.gray5;
    selfObject->gizmosIsFilled = false;
    selfObject->gizmosOpacity = 1.f;
    selfFillRect->blendMod.mode = SDL_BLENDMODE_BLEND;
    selfFillRect->blendMod.alpha = 1.f;

    // Virtual methods
    selfObject->onRender = UIFillRectVM_onRender;
    selfObject->onUpdate = UIFillRectVM_onUpdate;
    selfObject->onDestroy = UIFillRectVM_onDestroy;
    selfObject->onDrawGizmos = UIFillRectVM_onDrawGizmos;
}

void UIFillRectVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIFillRect* selfFillRect = (UIFillRect*)selfPtr;

    Transform transform = { 0 };
    RenderAnchor anchor = { 0 };
    RenderDim dim = { 0 };

    UITransform_getComponents(&(selfObj->transform), &transform, &dim, &anchor);
    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->layer);
    cmd.rect = &(selfFillRect->rect);
    cmd.anchor = &anchor;
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.blendMod = &(selfFillRect->blendMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}
