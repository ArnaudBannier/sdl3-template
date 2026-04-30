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

    selfObject->m_type |= UI_TYPE_FILL_RECT;
    selfFillRect->m_rect.filled = true;
    selfFillRect->m_rect.color = color;
    selfObject->m_gizmosColor = g_colors.gray5;
    selfObject->m_gizmosIsFilled = false;
    selfObject->m_gizmosOpacity = 1.f;
    selfFillRect->m_blendMod.mode = SDL_BLENDMODE_BLEND;
    selfFillRect->m_blendMod.alpha = 1.f;

    // Virtual methods
    selfObject->m_onRender = UIFillRectVM_onRender;
    selfObject->m_onUpdate = UIFillRectVM_onUpdate;
    selfObject->m_onDestroy = UIFillRectVM_onDestroy;
    selfObject->m_onDrawGizmos = UIFillRectVM_onDrawGizmos;
}

void UIFillRectVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIFillRect* selfFillRect = (UIFillRect*)selfPtr;

    Transform transform = { 0 };
    RenderAnchor anchor = { 0 };
    RenderDim dim = { 0 };

    UITransform_getComponents(&(selfObj->m_transform), &transform, &dim, &anchor);
    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->m_layer);
    cmd.rect = &(selfFillRect->m_rect);
    cmd.anchor = &anchor;
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.blendMod = &(selfFillRect->m_blendMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}
