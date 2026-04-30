/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_system.h"
#include "engine_common.h"

#define UI_SYSTEM_INVALID_ID -1

UISystem* UISystem_create(
    AssetManager* assets, AudioSystem* audioSystem,
    Viewport* viewport, float virtualWidth,
    int capacity)
{
    UISystem* self = (UISystem*)calloc(1, sizeof(UISystem));
    ASSERT_NEW(self);

    assert(viewport && "viewport must not be NULL");
    const float aspectRatio = Viewport_getAspectRatio(viewport);

    self->m_assetManager = assets;
    self->m_audioSystem = audioSystem;
    self->m_viewport = viewport;
    self->m_virtualWidth = virtualWidth;
    self->m_virtualHeight = virtualWidth / aspectRatio;
    self->m_audioGroupIndex = 0;

    self->m_capacity = capacity;
    self->m_uiObjectCount = 0;
    self->m_uiObjects = (UIObject**)calloc(capacity, sizeof(UIObject*));
    ASSERT_NEW(self->m_uiObjects);

    self->m_generations = (uint32_t*)calloc(capacity, sizeof(uint32_t));
    ASSERT_NEW(self->m_generations);

    self->m_sparseToDense = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_sparseToDense);

    self->m_denseToSparse = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_denseToSparse);

    self->m_nextFree = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_nextFree);

    for (int i = 0; i < capacity; i++)
    {
        self->m_generations[i] = 1;
    }

    // Initialize the free list
    for (int i = 0; i < capacity - 1; i++)
    {
        self->m_nextFree[i] = i + 1;
    }
    self->m_nextFree[capacity - 1] = -1; // End of free list
    self->m_firstFreeIndex = 0;

    // Create the root canvas object
    self->m_canvas = UIObject_create(self, "ui-system-canvas");
    UIObject_setGizmosColor(self->m_canvas, g_colors.lime5);

    return self;
}

void UISystem_destroy(UISystem* self)
{
    if (!self) return;
    if (self->m_uiObjects)
    {
        for (int i = 0; i < self->m_uiObjectCount; i++)
        {
        #if defined(_MSC_VER)
        #  pragma warning(push)
        #  pragma warning(disable:6001)
        #endif
            UIObject* object = self->m_uiObjects[i];
            assert(object && "UIObject in m_uiObjects should not be NULL");
            assert(object->m_onDestroy && "UIObject in m_uiObjects should have a valid m_onDestroy function");

            object->m_onDestroy(object);
            free(object);
        #if defined(_MSC_VER)
        #  pragma warning(pop)
        #endif
        }
        free(self->m_uiObjects);
    }

    free(self->m_generations);
    free(self->m_sparseToDense);
    free(self->m_denseToSparse);
    free(self->m_nextFree);
    free(self);
}

void UISystem_update(UISystem* self)
{
    const float aspectRatio = Viewport_getAspectRatio(self->m_viewport);
    self->m_virtualHeight = self->m_virtualWidth / aspectRatio;

    UIRect rect = { 0 };
    rect.anchorMin = Vec2_set(0.f, 0.f);
    rect.anchorMax = Vec2_set(0.f, 0.f);
    rect.offsetMin = Vec2_set(0.f, 0.f);
    rect.offsetMax.x = self->m_virtualWidth;
    rect.offsetMax.y = self->m_virtualHeight;
    UIObject_setRect(self->m_canvas, rect);

    // Mark all objects as orphaned before updating.
    for (int i = 0; i < self->m_uiObjectCount; i++)
    {
        self->m_uiObjects[i]->m_orphan = true;
    }
    // Clear orphan status for objects that are still in the tree.
    UIObject_clearOrphanStatus(self->m_canvas);

    // Update the UI tree
    UIObject_update(self->m_canvas);
}

void UISystem_render(UISystem* self, GraphicsSystem* graphicsSystem)
{
    // Render the UI tree
    UIObject_render(self->m_canvas, graphicsSystem);
}


void UISystem_drawGizmos(UISystem* self, GizmosSystem* gizmosSystem)
{
    // Draw gizmos for the UI tree
    UIObject_drawGizmos(self->m_canvas, gizmosSystem);
}

float UISystem_viewToUIX(const UISystem* self, float x)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->m_viewport);
    const float ratioX = x / viewportRect.w;
    return self->m_virtualWidth * ratioX;
}

float UISystem_viewToUIY(const UISystem* self, float y)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->m_viewport);
    const float ratioY = y / viewportRect.h;
    return self->m_virtualHeight * (1.f - ratioY);
}

float UISystem_viewToUILength(const UISystem* self, float length)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    const float viewToUIScale = self->m_virtualWidth / Viewport_getWidth(self->m_viewport);
    return length * viewToUIScale;
}

void UISystem_uiToView(const UISystem* self, Vec2 uiPosition, float* x, float* y)
{
    assert(self && "self must not be NULL");
    assert(x && y);
    (*x) = UISystem_uiToViewX(self, uiPosition.x);
    (*y) = UISystem_uiToViewY(self, uiPosition.y);
}

Vec2 UISystem_uiToViewV(const UISystem* self, Vec2 uiPosition)
{
    return Vec2_set(
        UISystem_uiToViewX(self, uiPosition.x),
        UISystem_uiToViewY(self, uiPosition.y)
    );
}

float UISystem_uiToViewX(const UISystem* self, float positionX)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->m_viewport);
    const float ratioX = positionX / self->m_virtualWidth;
    return ratioX * viewportRect.w;
}

float UISystem_uiToViewY(const UISystem* self, float positionY)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->m_viewport);
    const float ratioY = positionY / self->m_virtualHeight;
    return (1.f - ratioY) * viewportRect.h;
}

float UISystem_uiToViewLength(const UISystem* self, float length)
{
    assert(self && "self must not be NULL");
    assert(self->m_viewport && "viewport must not be NULL");

    const float uiToViewScale = Viewport_getWidth(self->m_viewport) / self->m_virtualWidth;
    return length * uiToViewScale;
}

bool UISystem_hasObjectId(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->m_capacity) return false;
    if (self->m_generations[id.index] != id.generation) return false;
    return true;
}

bool UISystem_hasObject(UISystem* self, UIObject* object)
{
    assert(self && "self must not be NULL");
    if (!object) return false;
    return UISystem_hasObjectId(self, object->m_id);
}

UIObject* UISystem_getObjectById(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (UIObjectId_isNull(id)) return NULL;
    if (id.index >= (uint32_t)self->m_capacity) return NULL;
    if (self->m_generations[id.index] != id.generation) return NULL;

    int denseIndex = self->m_sparseToDense[id.index];
    return self->m_uiObjects[denseIndex];
}

void UISystem_addObject(UISystem* self, UIObject* object)
{
    assert(self && "self must not be NULL");
    if (self->m_firstFreeIndex == -1)
    {
        assert(false && "UISystem capacity exceeded");
        return;
    }

    int sparseIndex = self->m_firstFreeIndex;
    int denseIndex = self->m_uiObjectCount++;
    self->m_firstFreeIndex = self->m_nextFree[sparseIndex];

    self->m_sparseToDense[sparseIndex] = denseIndex;
    self->m_denseToSparse[denseIndex] = sparseIndex;

    self->m_uiObjects[denseIndex] = object;

    object->m_id.index = (uint32_t)sparseIndex;
    object->m_id.generation = self->m_generations[sparseIndex];

    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "Add object %s to ui system\n",
        UIObject_getObjectName(object)
    );
    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "UISystem size: %d / %d\n",
        self->m_uiObjectCount, self->m_capacity
    );
}

void UISystem_removeObjectId(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->m_capacity) return;
    if (self->m_generations[id.index] != id.generation) return;

    int denseIndex = self->m_sparseToDense[id.index];
    int lastDenseIndex = self->m_uiObjectCount - 1;

    if (denseIndex != lastDenseIndex)
    {
        // Move the last body to the freed slot
        self->m_uiObjects[denseIndex] = self->m_uiObjects[lastDenseIndex];
        // Update mappings
        int movedSparseIndex = self->m_denseToSparse[lastDenseIndex];
        self->m_sparseToDense[movedSparseIndex] = denseIndex;
        self->m_denseToSparse[denseIndex] = movedSparseIndex;
    }

    self->m_uiObjectCount--;

    // Add the freed slot back to the free list
    self->m_nextFree[id.index] = self->m_firstFreeIndex;
    self->m_firstFreeIndex = id.index;

    // Increment the generation to invalidate old IDs
    self->m_generations[id.index]++;

    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "Remove one object from ui system\n"
    );
    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "UISystem size: %d / %d\n",
        self->m_uiObjectCount, self->m_capacity
    );
}

void UISystem_playSFX(UISystem* self, int audioId)
{
    assert(self && "self must not be NULL");
    assert(self->m_audioSystem && "audio system must not be NULL");
    if (audioId < 0) return;

    AssetManager* assets = self->m_assetManager;
    assert(assets && "asset manager must not be NULL");
    MIX_Audio* audio = AssetManager_getAudio(assets, audioId);
    assert(audio);

    AudioPlayOptions options = { 0 };
    AudioPlayOptions_init(&options);
    options.gain = AssetManager_getAudioGain(assets, audioId);

    AudioSystem_play(self->m_audioSystem, self->m_audioGroupIndex, audio, &options);
}
