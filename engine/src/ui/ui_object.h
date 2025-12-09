/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_utils.h"

typedef struct UIObject UIObject;

void UIObjectVM_onRender(void* self);
void UIObjectVM_onUpdate(void* self);
void UIObjectVM_onDestroy(void* self);

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

struct UIObject
{
    int m_magicNumber;
    UIType m_type;
    int m_objectId;
    char* m_objectName;
    bool m_enabled;

    UITransform m_transform;
    UIObject* m_parent;
    UIObject* m_firstChild;
    UIObject* m_nextSibling;

    void (*m_onRender)(void* self);
    void (*m_onUpdate)(void* self);
    void (*m_onDestroy)(void* self);
};

UIObject* UIObject_create(const char* objectName);
void UIObject_destroy(void* self);
void UIObject_init(void* self, const char* objectName);

void UIObject_setParent(void* self, void* parent);
void UIObject_setRect(void* self, UIRect rect);
void UIObject_setEnabled(void* self, bool enabled);
bool UIObject_isEnabled(void* self);
void UIObject_getViewportRect(void* self, SDL_FRect* outRect);
int UIObject_getObjectId(void* self);
const char* UIObject_getObjectName(void* self);

void UIObject_update(void* self);
void UIObject_render(void* self);

bool UIObject_isOfType(void* self, UIType type);

INLINE void UIObject_setRect(void* self, UIRect rect)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    ((UIObject*)self)->m_transform.localRect = rect;
}

INLINE void UIObject_setEnabled(void* self, bool enabled)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    ((UIObject*)self)->m_enabled = enabled;
}

INLINE bool UIObject_isEnabled(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)self)->m_enabled;
}

INLINE int UIObject_getObjectId(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)self)->m_objectId;
}

INLINE const char* UIObject_getObjectName(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    return ((UIObject*)self)->m_objectName;
}

INLINE void UIObject_getAABB(void* self, AABB* out)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)self;
    *out = selfObj->m_transform.aabb;
}

INLINE UIObject* UIObject_getParent(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)self;
    return selfObj->m_parent;
}

void UIObject_printHierarchy(void* self, int level);
