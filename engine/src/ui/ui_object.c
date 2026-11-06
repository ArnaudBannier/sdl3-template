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

    selfObj->magicNumber = MAGIC_NUMBER_UI_OBJECT;
    selfObj->type = UI_TYPE_OBJECT;
    selfObj->enabled = true;
    selfObj->orphan = true;
    selfObj->objectName = SDL_strdup(objectName);
    selfObj->uiSystem = uiSystem;

    selfObj->parentId = UIObjectId_null;
    selfObj->firstChildId = UIObjectId_null;
    selfObj->nextSiblingId = UIObjectId_null;

    selfObj->transform.localRect.anchorMin = Vec2_set(0.f, 0.f);
    selfObj->transform.localRect.anchorMax = Vec2_set(1.f, 1.f);
    selfObj->transform.localRect.offsetMin = Vec2_set(0.f, 0.f);
    selfObj->transform.localRect.offsetMax = Vec2_set(0.f, 0.f);

    selfObj->layer.layer = DEFAULT_UI_LAYER;
    selfObj->layer.orderInLayer = 0;

    selfObj->gizmosColor = g_colors.cyan5;
    selfObj->gizmosIsFilled = false;
    selfObj->gizmosOpacity = 1.0f;

    UISystem_addObject(uiSystem, selfObj);

    // Virtual methods
    selfObj->onRender = UIObjectVM_onRender;
    selfObj->onDrawGizmos = UIObjectVM_onDrawGizmos;
    selfObj->onUpdate = UIObjectVM_onUpdate;
    selfObj->onDestroy = UIObjectVM_onDestroy;
}

static void UIObject_destroyRecursive(UIObject* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObjectId childId = self->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(self->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

        UIObject_destroyRecursive(child);
    }

    // Appel du destructeur virtuel
    self->onDestroy(self);

    UISystem_removeObjectId(self->uiSystem, self->id);

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
    if (obj->magicNumber != MAGIC_NUMBER_UI_OBJECT) return false;
    return (obj->type & type) != 0;
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
        UIObject_setParentId(selfPtr, parentObj->id);
    }
}

void UIObject_setParentId(void* selfPtr, UIObjectId parentId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    UIObject* selfObj = (UIObject*)selfPtr;
    if (UIObjectId_equal(selfObj->parentId, parentId)) return;

    UIObject* currParent = UISystem_getObjectById(selfObj->uiSystem, selfObj->parentId);
    UIObject* nextParent = UISystem_getObjectById(selfObj->uiSystem, parentId);
    if (currParent)
    {
        UIObject* prevChild = NULL;
        UIObject* currChild = UISystem_getObjectById(selfObj->uiSystem, currParent->firstChildId);
        while (currChild)
        {
            if (currChild == selfObj)
            {
                if (prevChild)
                {
                    prevChild->nextSiblingId = currChild->nextSiblingId;
                }
                else
                {
                    currParent->firstChildId = currChild->nextSiblingId;
                }
                break;
            }
            prevChild = currChild;
            currChild = UISystem_getObjectById(selfObj->uiSystem, currChild->nextSiblingId);
        }
    }
    selfObj->parentId = parentId;
    if (nextParent)
    {
        selfObj->nextSiblingId = nextParent->firstChildId;
        nextParent->firstChildId = selfObj->id;
    }
}

UIObject* UIObject_getParent(const void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    return UISystem_getObjectById(selfObj->uiSystem, selfObj->parentId);
}

void UIObject_update(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->enabled == false) return;

    selfObj->onUpdate(selfObj);

    UIObjectId childId = selfObj->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

        UIObject_update(child);
    }
}

void UIObject_render(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->enabled == false) return;

    selfObj->onRender(selfObj, graphicsSystem);

    UIObjectId childId = selfObj->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

        UIObject_render(child, graphicsSystem);
    }
}

void UIObject_drawGizmos(void* selfPtr, GizmosSystem* gizmosSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");

    if (selfObj->enabled == false) return;

    selfObj->onDrawGizmos(selfObj, gizmosSystem);

    UIObjectId childId = selfObj->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

        UIObject_drawGizmos(child, gizmosSystem);
    }
}

void UIObject_clearOrphanStatus(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    selfObj->orphan = false;

    UIObjectId childId = selfObj->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

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

    UITransform_getGizmos(&selfObj->transform, &transform, &gizmosRect);
    blendMod.mode = SDL_BLENDMODE_BLEND;
    blendMod.alpha = selfObj->gizmosOpacity;
    gizmosRect.filled = selfObj->gizmosIsFilled;

    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.rect = &gizmosRect;
    cmd.color = &selfObj->gizmosColor;
    cmd.blendMod = &blendMod;
    GizmosSystem_addCommand(gizmosSystem, &cmd);
}

void UIObjectVM_onUpdate(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIObject* parent = UISystem_getObjectById(selfObj->uiSystem, selfObj->parentId);
    UITransform_updateAABB(&selfObj->transform, parent ? &parent->transform : NULL);
}

void UIObjectVM_onDestroy(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    SDL_free(selfObj->objectName);
}

void UIObject_getViewportRect(void* selfPtr, SDL_FRect* outRect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;

    UITransform_getViewportRect(&selfObj->transform, selfObj->uiSystem, outRect);
}

void UIObject_printHierarchy(void* selfPtr, int level)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_OBJECT) && "self must be of type UI_TYPE_OBJECT");
    UIObject* selfObj = (UIObject*)selfPtr;
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }
    printf("- %s (Type: 0x%04X)\n", selfObj->objectName, selfObj->type);

    UIObjectId childId = selfObj->firstChildId;
    while (UIObjectId_isNull(childId) == false)
    {
        UIObject* child = UISystem_getObjectById(selfObj->uiSystem, childId);
        if (child == NULL)
        {
            // This should not happen, but if it does, we break the loop to avoid an infinite loop.
            assert(false && "Child object not found in UISystem");
            break;
        }
        childId = child->nextSiblingId;

        UIObject_printHierarchy(child, level + 1);
    }
}
