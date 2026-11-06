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

    self->assetManager = assets;
    self->audioSystem = audioSystem;
    self->viewport = viewport;
    self->virtualWidth = virtualWidth;
    self->virtualHeight = virtualWidth / aspectRatio;
    self->audioGroupIndex = 0;

    self->capacity = capacity;
    self->uiObjectCount = 0;
    self->uiObjects = (UIObject**)calloc(capacity, sizeof(UIObject*));
    ASSERT_NEW(self->uiObjects);

    self->generations = (uint32_t*)calloc(capacity, sizeof(uint32_t));
    ASSERT_NEW(self->generations);

    self->sparseToDense = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->sparseToDense);

    self->denseToSparse = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->denseToSparse);

    self->nextFree = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->nextFree);

    for (int i = 0; i < capacity; i++)
    {
        self->generations[i] = 1;
    }

    // Initialize the free list
    for (int i = 0; i < capacity - 1; i++)
    {
        self->nextFree[i] = i + 1;
    }
    self->nextFree[capacity - 1] = -1; // End of free list
    self->firstFreeIndex = 0;

    // Create the root canvas object
    self->canvas = UIObject_create(self, "ui-system-canvas");
    UIObject_setGizmosColor(self->canvas, g_colors.lime5);

    return self;
}

void UISystem_destroy(UISystem* self)
{
    if (!self) return;
    if (self->uiObjects)
    {
        for (int i = 0; i < self->uiObjectCount; i++)
        {
        #if defined(_MSC_VER)
        #  pragma warning(push)
        #  pragma warning(disable:6001)
        #endif
            UIObject* object = self->uiObjects[i];
            assert(object && "UIObject in uiObjects should not be NULL");
            assert(object->onDestroy && "UIObject in uiObjects should have a valid onDestroy function");

            object->onDestroy(object);
            free(object);
        #if defined(_MSC_VER)
        #  pragma warning(pop)
        #endif
        }
        free(self->uiObjects);
    }

    free(self->generations);
    free(self->sparseToDense);
    free(self->denseToSparse);
    free(self->nextFree);
    free(self);
}

void UISystem_update(UISystem* self)
{
    const float aspectRatio = Viewport_getAspectRatio(self->viewport);
    self->virtualHeight = self->virtualWidth / aspectRatio;

    UIRect rect = { 0 };
    rect.anchorMin = Vec2_set(0.f, 0.f);
    rect.anchorMax = Vec2_set(0.f, 0.f);
    rect.offsetMin = Vec2_set(0.f, 0.f);
    rect.offsetMax.x = self->virtualWidth;
    rect.offsetMax.y = self->virtualHeight;
    UIObject_setRect(self->canvas, rect);

    // Mark all objects as orphaned before updating.
    for (int i = 0; i < self->uiObjectCount; i++)
    {
        self->uiObjects[i]->orphan = true;
    }
    // Clear orphan status for objects that are still in the tree.
    UIObject_clearOrphanStatus(self->canvas);

    // Update the UI tree
    UIObject_update(self->canvas);
}

void UISystem_render(UISystem* self, GraphicsSystem* graphicsSystem)
{
    // Render the UI tree
    UIObject_render(self->canvas, graphicsSystem);
}


void UISystem_drawGizmos(UISystem* self, GizmosSystem* gizmosSystem)
{
    // Draw gizmos for the UI tree
    UIObject_drawGizmos(self->canvas, gizmosSystem);
}

float UISystem_viewToUIX(const UISystem* self, float x)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->viewport);
    const float ratioX = x / viewportRect.w;
    return self->virtualWidth * ratioX;
}

float UISystem_viewToUIY(const UISystem* self, float y)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->viewport);
    const float ratioY = y / viewportRect.h;
    return self->virtualHeight * (1.f - ratioY);
}

float UISystem_viewToUILength(const UISystem* self, float length)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    const float viewToUIScale = self->virtualWidth / Viewport_getWidth(self->viewport);
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
    assert(self->viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->viewport);
    const float ratioX = positionX / self->virtualWidth;
    return ratioX * viewportRect.w;
}

float UISystem_uiToViewY(const UISystem* self, float positionY)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->viewport);
    const float ratioY = positionY / self->virtualHeight;
    return (1.f - ratioY) * viewportRect.h;
}

float UISystem_uiToViewLength(const UISystem* self, float length)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    const float uiToViewScale = Viewport_getWidth(self->viewport) / self->virtualWidth;
    return length * uiToViewScale;
}

bool UISystem_hasObjectId(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->capacity) return false;
    if (self->generations[id.index] != id.generation) return false;
    return true;
}

bool UISystem_hasObject(UISystem* self, UIObject* object)
{
    assert(self && "self must not be NULL");
    if (!object) return false;
    return UISystem_hasObjectId(self, object->id);
}

UIObject* UISystem_getObjectById(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (UIObjectId_isNull(id)) return NULL;
    if (id.index >= (uint32_t)self->capacity) return NULL;
    if (self->generations[id.index] != id.generation) return NULL;

    int denseIndex = self->sparseToDense[id.index];
    return self->uiObjects[denseIndex];
}

void UISystem_addObject(UISystem* self, UIObject* object)
{
    assert(self && "self must not be NULL");
    if (self->firstFreeIndex == -1)
    {
        assert(false && "UISystem capacity exceeded");
        return;
    }

    int sparseIndex = self->firstFreeIndex;
    int denseIndex = self->uiObjectCount++;
    self->firstFreeIndex = self->nextFree[sparseIndex];

    self->sparseToDense[sparseIndex] = denseIndex;
    self->denseToSparse[denseIndex] = sparseIndex;

    self->uiObjects[denseIndex] = object;

    object->id.index = (uint32_t)sparseIndex;
    object->id.generation = self->generations[sparseIndex];

    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "Add object %s to ui system\n",
        UIObject_getObjectName(object)
    );
    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "UISystem size: %d / %d\n",
        self->uiObjectCount, self->capacity
    );
}

void UISystem_removeObjectId(UISystem* self, UIObjectId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->capacity) return;
    if (self->generations[id.index] != id.generation) return;

    int denseIndex = self->sparseToDense[id.index];
    int lastDenseIndex = self->uiObjectCount - 1;

    if (denseIndex != lastDenseIndex)
    {
        // Move the last body to the freed slot
        self->uiObjects[denseIndex] = self->uiObjects[lastDenseIndex];
        // Update mappings
        int movedSparseIndex = self->denseToSparse[lastDenseIndex];
        self->sparseToDense[movedSparseIndex] = denseIndex;
        self->denseToSparse[denseIndex] = movedSparseIndex;
    }

    self->uiObjectCount--;

    // Add the freed slot back to the free list
    self->nextFree[id.index] = self->firstFreeIndex;
    self->firstFreeIndex = id.index;

    // Increment the generation to invalidate old IDs
    self->generations[id.index]++;

    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "Remove one object from ui system\n"
    );
    SDL_LogInfo(
        SDL_LOG_CATEGORY_SYSTEM,
        "UISystem size: %d / %d\n",
        self->uiObjectCount, self->capacity
    );
}

void UISystem_playSFX(UISystem* self, int audioId)
{
    assert(self && "self must not be NULL");
    assert(self->audioSystem && "audio system must not be NULL");
    if (audioId < 0) return;

    AssetManager* assets = self->assetManager;
    assert(assets && "asset manager must not be NULL");
    MIX_Audio* audio = AssetManager_getAudio(assets, audioId);
    assert(audio);

    AudioPlayOptions options = { 0 };
    AudioPlayOptions_init(&options);
    options.gain = AssetManager_getAudioGain(assets, audioId);

    AudioSystem_play(self->audioSystem, self->audioGroupIndex, audio, &options);
}
