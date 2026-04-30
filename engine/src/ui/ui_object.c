/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_object.h"
#include "ui/ui_system.h"
#include "engine_common.h"

const UIObjectId UIObjectId_null = { 0, 0 };

UIObject* UIObject_create(UISystem* uiSystem, const char* objectName)
{
    UIObject* self = (UIObject*)calloc(1, sizeof(UIObject));
    ASSERT_NEW(self);

    UIObject_init(self, uiSystem, objectName);

    return self;
}

void UIObject_init(void* selfPtr, UISystem* uiSystem, const char* objectName)
{
    assert(selfPtr && "self must not be NULL");
    assert(uiSystem && "uiSystem must not be NULL");
    assert(objectName && "objectName must not be NULL");
    UIObject* selfObj = (UIObject*)selfPtr;

    selfObj->m_magicNumber = MAGIC_NUMBER_UI_OBJECT;
    selfObj->m_type = UI_TYPE_OBJECT;
    selfObj->m_enabled = true;
    selfObj->m_orphan = true;
    selfObj->m_objectName = SDL_strdup(objectName);
    selfObj->m_uiSystem = uiSystem;

    selfObj->m_parentId = UIObjectId_null;
    selfObj->m_firstChildId = UIObjectId_null;
    selfObj->m_nextSiblingId = UIObjectId_null;

    selfObj->m_transform.localRect.anchorMin = Vec2_set(0.f, 0.f);
    selfObj->m_transform.localRect.anchorMax = Vec2_set(1.f, 1.f);
    selfObj->m_transform.localRect.offsetMin = Vec2_set(0.f, 0.f);
    selfObj->m_transform.localRect.offsetMax = Vec2_set(0.f, 0.f);

    selfObj->m_layer.layer = DEFAULT_UI_LAYER;
    selfObj->m_layer.orderInLayer = 0;

    selfObj->m_gizmosColor = g_colors.cyan5;
    selfObj->m_gizmosIsFilled = false;
    selfObj->m_gizmosOpacity = 1.0f;

    UISystem_addObject(uiSystem, selfObj);

    // Virtual methods
    selfObj->m_onRender = UIObjectVM_onRender;
    selfObj->m_onDrawGizmos = UIObjectVM_onDrawGizmos;
    selfObj->m_onUpdate = UIObjectVM_onUpdate;
    selfObj->m_onDestroy = UIObjectVM_onDestroy;
}

static void UIObject_destroyRecursive(UIObject* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObjectId childId = self->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(self->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_destroyRecursive(child);
    }

    // Appel du destructeur virtuel
    self->m_onDestroy(self);

    UISystem_removeObjectId(self->m_uiSystem, self->m_id);

    free(self);
}

void UIObject_destroy(void* selfPtr)
{
    if (!selfPtr) return;

    UIObject_setParentId(selfPtr, UIObjectId_null);
    UIObject_destroyRecursive(selfPtr);
}

bool UIObject_isOfType(const void* selfPtr, UIType type)
{
    if (!selfPtr) return false;
    const UIObject* obj = (UIObject*)selfPtr;
    if (obj->m_magicNumber != MAGIC_NUMBER_UI_OBJECT) return false;
    return (obj->m_type & type) != 0;
}

void UIObject_setParent(void* selfPtr, void* parent)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    if (parent == NULL)
    {
        UIObject_setParentId(selfPtr, UIObjectId_null);
    }
    else
    {
        assert(UIObject_isOfType(parent, UI_TYPE_OBJECT) && "parent must be of type UI_TYPE_OBJECT");
        UIObject* parentObj = (UIObject*)parent;
        UIObject_setParentId(selfPtr, parentObj->m_id);
    }
}

void UIObject_setParentId(void* selfPtr, UIObjectId parentId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObject* selfObj = (UIObject*)selfPtr;
    if (UIObjectId_equal(selfObj->m_parentId, parentId)) return;

    UIObject* currParent = UISystem_getObjectById(selfObj->m_uiSystem, selfObj->m_parentId);
    UIObject* nextParent = UISystem_getObjectById(selfObj->m_uiSystem, parentId);
    if (currParent)
    {
        UIObject* prevChild = NULL;
        UIObject* currChild = UISystem_getObjectById(selfObj->m_uiSystem, currParent->m_firstChildId);
        while (currChild)
        {
            if (currChild == selfObj)
            {
                if (prevChild)
                {
                    prevChild->m_nextSiblingId = currChild->m_nextSiblingId;
                }
                else
                {
                    currParent->m_firstChildId = currChild->m_nextSiblingId;
                }
                break;
            }
            prevChild = currChild;
            currChild = UISystem_getObjectById(selfObj->m_uiSystem, currChild->m_nextSiblingId);
        }
    }
    selfObj->m_parentId = parentId;
    if (nextParent)
    {
        selfObj->m_nextSiblingId = nextParent->m_firstChildId;
        nextParent->m_firstChildId = selfObj->m_id;
    }
}

UIObject* UIObject_getParent(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    return UISystem_getObjectById(selfObj->m_uiSystem, selfObj->m_parentId);
}

void UIObject_update(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->m_enabled == false) return;

    selfObj->m_onUpdate(selfObj);

    UIObjectId childId = selfObj->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_update(child);
    }
}

void UIObject_render(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->m_enabled == false) return;

    selfObj->m_onRender(selfObj, graphicsSystem);

    UIObjectId childId = selfObj->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_render(child, graphicsSystem);
    }
}

void UIObject_drawGizmos(void* selfPtr, GizmosSystem* gizmosSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->m_enabled == false) return;

    selfObj->m_onDrawGizmos(selfObj, gizmosSystem);

    UIObjectId childId = selfObj->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_drawGizmos(child, gizmosSystem);
    }
}

void UIObject_clearOrphanStatus(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->m_orphan = false;

    UIObjectId childId = selfObj->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_clearOrphanStatus(child);
    }
}

void UIObjectVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    // Base UIObject does not render anything by default,
    // but derived types can override this method to render themselves.
}

void UIObjectVM_onDrawGizmos(void* selfPtr, GizmosSystem* gizmosSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    Transform transform = { 0 };
    GizmosRect gizmosRect = { 0 };
    RenderBlendMod blendMod = { 0 };

    UITransform_getGizmos(&selfObj->m_transform, &transform, &gizmosRect);
    blendMod.mode = SDL_BLENDMODE_BLEND;
    blendMod.alpha = selfObj->m_gizmosOpacity;
    gizmosRect.filled = selfObj->m_gizmosIsFilled;

    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.rect = &gizmosRect;
    cmd.color = &selfObj->m_gizmosColor;
    cmd.blendMod = &blendMod;
    GizmosSystem_addCommand(gizmosSystem, &cmd);
}

void UIObjectVM_onUpdate(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIObject* parent = UISystem_getObjectById(selfObj->m_uiSystem, selfObj->m_parentId);
    UITransform_updateAABB(&selfObj->m_transform, parent ? &parent->m_transform : NULL);
}

void UIObjectVM_onDestroy(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    SDL_free(selfObj->m_objectName);
}

void UIObject_getViewportRect(void* selfPtr, SDL_FRect* outRect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;

    UITransform_getViewportRect(&selfObj->m_transform, selfObj->m_uiSystem, outRect);
}

void UIObject_printHierarchy(void* selfPtr, int level)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }
    printf("- %s (Type: 0x%04X)\n", selfObj->m_objectName, selfObj->m_type);

    UIObjectId childId = selfObj->m_firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->m_uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->m_nextSiblingId;

        UIObject_printHierarchy(child, level + 1);
    }
}
