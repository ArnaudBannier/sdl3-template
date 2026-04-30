/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/graphics_system.h"
#include "core/gizmos_system.h"
#include "ui/ui_utils.h"

typedef struct UIObject UIObject;
typedef struct UISystem UISystem;

void UIObjectVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
void UIObjectVM_onDrawGizmos(void* selfPtr, GizmosSystem* gizmosSystem);
void UIObjectVM_onUpdate(void* selfPtr);
void UIObjectVM_onDestroy(void* selfPtr);

typedef enum UIType
{
    UI_TYPE_OBJECT = 0x0001,
    UI_TYPE_CANVAS = 0x0002,
    UI_TYPE_SELECTABLE = 0x0004,
    UI_TYPE_BUTTON = 0x0008,
    UI_TYPE_LIST = 0x0010,
    UI_TYPE_GRID_LAYOUT = 0x0020,
    UI_TYPE_LABEL = 0x0040,
    UI_TYPE_IMAGE = 0x0080,
    UI_TYPE_FILL_RECT = 0x0100,
} UIType;

typedef struct UIObjectId
{
    uint32_t index;
    uint32_t generation;
} UIObjectId;

INLINE bool UIObjectId_equal(UIObjectId a, UIObjectId b)
{
    return a.index == b.index && a.generation == b.generation;
}

INLINE bool UIObjectId_isNull(UIObjectId id)
{
    return id.generation == 0;
}

extern const UIObjectId UIObjectId_null;

struct UIObject
{
    uint64_t m_magicNumber;
    UIType m_type;
    char* m_objectName;
    bool m_enabled;

    bool m_orphan;
    UISystem* m_uiSystem;
    UIObjectId m_id;

    RenderSortingLayer m_layer;
    UITransform m_transform;
    UIObjectId m_parentId;
    UIObjectId m_firstChildId;
    UIObjectId m_nextSiblingId;

    SDL_Color m_gizmosColor;
    bool m_gizmosIsFilled;
    float m_gizmosOpacity;

    void (*m_onRender)(void* selfPtr, GraphicsSystem* graphicsSystem);
    void (*m_onDrawGizmos)(void* selfPtr, GizmosSystem* gizmosSystem);
    void (*m_onUpdate)(void* selfPtr);
    void (*m_onDestroy)(void* selfPtr);
};

UIObject* UIObject_create(UISystem* uiSystem, const char* objectName);
void UIObject_destroy(void* selfPtr);
void UIObject_init(void* selfPtr, UISystem* uiSystem, const char* objectName);

void UIObject_setParent(void* selfPtr, void* parent);
void UIObject_setParentId(void* selfPtr, UIObjectId parentId);
void UIObject_getViewportRect(void* selfPtr, SDL_FRect* outRect);

void UIObject_update(void* selfPtr);
void UIObject_render(void* selfPtr, GraphicsSystem* graphicsSystem);
void UIObject_drawGizmos(void* selfPtr, GizmosSystem* gizmosSystem);
void UIObject_clearOrphanStatus(void* selfPtr);

bool UIObject_isOfType(const void* selfPtr, UIType type);

INLINE void UIObject_setRect(void* selfPtr, UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    ((UIObject*)selfPtr)->m_transform.localRect = rect;
}

INLINE void UIObject_setEnabled(void* selfPtr, bool enabled)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    ((UIObject*)selfPtr)->m_enabled = enabled;
}

INLINE bool UIObject_isEnabled(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)selfPtr)->m_enabled;
}

INLINE UIObjectId UIObject_getObjectId(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)selfPtr)->m_id;
}

INLINE UIObjectId UIObject_getParentId(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)selfPtr)->m_parentId;
}

UIObject* UIObject_getParent(const void* selfPtr);

INLINE const char* UIObject_getObjectName(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)selfPtr)->m_objectName;
}

INLINE void UIObject_getAABB(const void* selfPtr, AABB* out)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    *out = selfObj->m_transform.aabb;
}

INLINE UISystem* UIObject_getUISystem(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    return selfObj->m_uiSystem;
}

INLINE RenderSortingLayer UIObject_getRenderLayer(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    return selfObj->m_layer;
}

INLINE void UIObject_setRenderLayer(void* selfPtr, RenderSortingLayer layer)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->m_layer = layer;
}

INLINE void UIObject_setGizmosColor(void* selfPtr, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->m_gizmosColor = color;
}

INLINE void UIObject_setGizmosFilled(void* selfPtr, bool isFilled)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->m_gizmosIsFilled = isFilled;
}

INLINE void UIObject_setGizmosOpacity(void* selfPtr, float opacity)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->m_gizmosOpacity = opacity;
}

void UIObject_printHierarchy(void* selfPtr, int level);
