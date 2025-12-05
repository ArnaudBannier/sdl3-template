/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_object.h"
#include "game_engine_common.h"

#define UI_OBJECT_MAGIC_NUMBER 0x1234ABCD

UIObject* UIObject_create(const char* objectName)
{
    UIObject* self = (UIObject*)calloc(1, sizeof(UIObject));
    AssertNew(self);

    UIObject_init(self, objectName);

    return self;
}

void UIObject_init(void* self, const char* objectName)
{
    assert(self && "self must not be NULL");
    static int s_nextObjectId = 1;
    UIObject* selfObj = (UIObject*)self;

    selfObj->m_magicNumber = UI_OBJECT_MAGIC_NUMBER;
    selfObj->m_type = UI_TYPE_OBJECT;
    selfObj->m_enabled = true;
    selfObj->m_objectId = s_nextObjectId++;
    selfObj->m_objectName = SDL_strdup(objectName);

    selfObj->m_transform.localRect.anchorMin = Vec2_set(0.f, 0.f);
    selfObj->m_transform.localRect.anchorMax = Vec2_set(1.f, 1.f);
    selfObj->m_transform.localRect.offsetMin = Vec2_set(0.f, 0.f);
    selfObj->m_transform.localRect.offsetMax = Vec2_set(0.f, 0.f);

    // Virtual methods
    selfObj->m_onRender = UIObjectVM_onRender;
    selfObj->m_onUpdate = UIObjectVM_onUpdate;
    selfObj->m_onDestroy = UIObjectVM_onDestroy;
}

static void UIObject_destroyRecursive(UIObject* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObject* child = self->m_firstChild;
    while (child)
    {
        UIObject* next = child->m_nextSibling;
        UIObject_destroyRecursive(child);
        child = next;
    }

    // Appel du destructeur virtuel
    self->m_onDestroy(self);

    free(self);
}

void UIObject_destroy(void* self)
{
    if (!self) return;

    UIObject_setParent(self, NULL);
    UIObject_destroyRecursive(self);
}

bool UIObject_isOfType(void* self, UIType type)
{
    if (!self) return false;
    UIObject* obj = (UIObject*)self;
    if (obj->m_magicNumber != UI_OBJECT_MAGIC_NUMBER) return false;
    return (obj->m_type & type) != 0;
}

void UIObject_setParent(void* self, void* parent)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObject* selfObj = (UIObject*)self;
    UIObject* parentObj = (UIObject*)parent;

    if (selfObj->m_parent)
    {
        UIObject* prevChild = NULL;
        UIObject* currChild = selfObj->m_parent->m_firstChild;
        while (currChild)
        {
            if (currChild == selfObj)
            {
                if (prevChild)
                {
                    prevChild->m_nextSibling = currChild->m_nextSibling;
                }
                else
                {
                    selfObj->m_parent->m_firstChild = currChild->m_nextSibling;
                }
                break;
            }
            prevChild = currChild;
            currChild = currChild->m_nextSibling;
        }
    }
    selfObj->m_parent = parentObj;
    if (parentObj)
    {
        selfObj->m_nextSibling = parentObj->m_firstChild;
        parentObj->m_firstChild = selfObj;
    }
}


void UIObject_update(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->m_enabled == false) return;

    selfObj->m_onUpdate(selfObj);
    for (UIObject* child = selfObj->m_firstChild; child != NULL; child = child->m_nextSibling)
    {
        UIObject_update(child);
    }
}

void UIObject_render(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->m_enabled == false) return;

    selfObj->m_onRender(selfObj);
    for (UIObject* child = selfObj->m_firstChild; child != NULL; child = child->m_nextSibling)
    {
        UIObject_render(child);
    }
}

void UIObjectVM_onRender(void* self)
{
    if (!g_drawUIGizmos) return;
    UIObject* selfObj = (UIObject*)self;
    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(selfObj, &viewportRect);
    SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
    SDL_RenderRect(g_renderer, &viewportRect);
}

void UIObjectVM_onUpdate(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UITransform_updateAABB(&selfObj->m_transform, selfObj->m_parent ? &selfObj->m_parent->m_transform : NULL);
}

void UIObjectVM_onDestroy(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    //printf("Freeing UIObject '%s'\n", selfObj->m_objectName);
    SDL_free(selfObj->m_objectName);
}

void UIObject_getViewportRect(void* self, SDL_FRect* outRect)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)self;

    UITransform_getViewportRect(&selfObj->m_transform, outRect);
}

void UIObject_printHierarchy(void* self, int level)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)self;
    for (int i = 0; i < level; ++i)
    {
        printf("  ");
    }
    printf("- %s (ID: %d, Type: 0x%04X)\n", selfObj->m_objectName, selfObj->m_objectId, selfObj->m_type);
    for (UIObject* child = selfObj->m_firstChild; child != NULL; child = child->m_nextSibling)
    {
        UIObject_printHierarchy(child, level + 1);
    }
}
