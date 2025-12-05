/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_canvas.h"
#include "game_engine_common.h"

static inline uint64_t hashSplitmix64(uint64_t value)
{
    uint64_t z = value + 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return z;
}

UICanvas* UICanvas_create(const char* objectName)
{
    UICanvas* self = (UICanvas*)calloc(1, sizeof(UICanvas));
    AssertNew(self);

    UICanvas_init(self, objectName);

    return self;
}

void UICanvas_init(void* self, const char* objectName)
{
    assert(self && "self must not be NULL");

    UIObject* selfObject = (UIObject*)self;
    UICanvas* selfCanvas = (UICanvas*)self;
    UIObject_init(selfObject, objectName);

    selfObject->m_type |= UI_TYPE_CANVAS;
    selfCanvas->m_setSize = 0;

    UIRect rect = { 0 };
    rect.anchorMin = Vec2_set(0.f, 0.f);
    rect.anchorMax = Vec2_set(0.f, 0.f);
    rect.offsetMin = Vec2_set(0.f, 0.f);
    rect.offsetMax = g_sizes.uiSize;
    selfObject->m_transform.localRect = rect;
    UITransform_updateAABB(&selfObject->m_transform, NULL);

    // Virtual methods
    selfObject->m_onRender = UICanvasVM_onRender;
    selfObject->m_onUpdate = UICanvasVM_onUpdate;
    selfObject->m_onDestroy = UICanvasVM_onDestroy;
}

static void UICanvas_recordObjectsRec(UICanvas* self, UIObject* object)
{
    if (self->m_setSize >= UI_CANVAS_MAX_ELEMENTS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Maximum number of canvas elements reached");
        assert(false);
        return;
    }

    int id = UIObject_getObjectId(object);
    uint64_t hash = hashSplitmix64((uint64_t)id);
    size_t index = hash % UI_CANVAS_SET_CAPACITY;
    while (self->m_set[index] != 0)
    {
        if (self->m_set[index] == id)
        {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Object is already recorded in the canvas");
            index = UI_CANVAS_SET_CAPACITY;
            break;
        }
        index = (index + 1) % UI_CANVAS_SET_CAPACITY;
    }
    if (index < UI_CANVAS_SET_CAPACITY)
    {
        self->m_set[index] = id;
        self->m_setSize++;
    }

    for (UIObject* child = object->m_firstChild; child != NULL; child = child->m_nextSibling)
    {
        UICanvas_recordObjectsRec(self, child);
    }
}

void UICanvas_recordObjects(UICanvas* self)
{
    UICanvas_recordObjectsRec(self, (UIObject*)self);
}

void UICanvasVM_onUpdate(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_CANVAS) && "self must be of type UI_TYPE_CANVAS");
    UICanvas* selfCanvas = (UICanvas*)self;

    UIObjectVM_onUpdate(self);

    // Clear the recorded set
    selfCanvas->m_setSize = 0;
    SDL_memset(selfCanvas->m_set, 0, sizeof(selfCanvas->m_set));

    // Record all objects in the canvas
    UICanvas_recordObjects(selfCanvas);
}

void UICanvasVM_onRender(void* self)
{
    if (!g_drawUIGizmos) return;
    UIObject* selfObj = (UIObject*)self;
    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(selfObj, &viewportRect);
    SDL_SetRenderDrawColor(g_renderer, 255, 128, 0, 255);
    SDL_RenderRect(g_renderer, &viewportRect);
}

bool UICanvas_hasObject(void* self, int id)
{
    assert(UIObject_isOfType(self, UI_TYPE_CANVAS) && "self must be of type UI_TYPE_CANVAS");
    UICanvas* selfCanvas = (UICanvas*)self;

    uint64_t hash = hashSplitmix64((uint64_t)id);
    size_t index = hash % UI_CANVAS_SET_CAPACITY;
    while (selfCanvas->m_set[index] != 0)
    {
        if (selfCanvas->m_set[index] == id)
        {
            return true;
        }
        index = (index + 1) % UI_CANVAS_SET_CAPACITY;
    }
    return false;
}
