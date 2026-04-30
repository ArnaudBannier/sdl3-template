/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/viewport.h"
#include "core/asset_manager.h"
#include "core/audio_system.h"
#include "core/graphics_system.h"
#include "core/gizmos_system.h"
#include "utils/utils.h"
#include "ui/ui_object.h"

/// @brief Structure représentant le système UI.
/// Gère les conversions entre coordonnées UI virtuelles (640×360) et viewport.
typedef struct UISystem
{
    AssetManager* m_assetManager;

    AudioSystem* m_audioSystem;

    /// @brief Pointeur vers le viewport utilisé (non possédé).
    Viewport* m_viewport;

    int m_audioGroupIndex;

    /// @brief Largeur virtuelle en pixels.
    float m_virtualWidth;

    /// @brief Hauteur virtuelle en pixels.
    float m_virtualHeight;

    UIObject* m_canvas;

    /// @brief Pointer to an array of uiObjects.
    /// This is the dense array where active uiObjects are stored.
    UIObject** m_uiObjects;

    /// @brief Pointer to an array of generation counters for each uiObject slot.
    /// Used to validate UIObjectIds. When a uiObject is destroyed, its generation is incremented to invalidate old IDs.
    /// This array is indexed by the sparse index from UIObjectId.
    uint32_t* m_generations;

    /// @brief Pointer to an array mapping sparse indices (from UIObjectId) to dense indices (in m_uiObjects).
    int* m_sparseToDense;

    /// @brief Pointer to an array mapping dense indices (in m_uiObjects) back to sparse indices (for UIObjectId).
    int* m_denseToSparse;

    /// @brief The current number of active uiObjects in the UI system.
    /// This is also the next dense index to use when creating a new uiObject.
    int m_uiObjectCount;

    /// @brief The maximum capacity of uiObjects that the UI system can handle.
    int m_capacity;

    /// @brief The head of the free list for uiObject slots.
    /// This is an index into the arrays that indicates the next free slot.
    int m_firstFreeIndex;

    /// @brief Pointer to an array that forms a linked list of free sparse indices.
    /// When a uiObject is destroyed, its sparse index is added to this free list.
    int* m_nextFree;
} UISystem;

/// @brief Crée un nouveau système UI.
/// @param assets le gestionnaire d'assets du jeu (non possédé par le système UI).
/// @param audioSystem le système audio du jeu (non possédé par le système UI).
/// @param viewport le viewport à utiliser (non possédé par le système UI).
/// @param virtualWidth la largeur virtuelle de l'UI (en pixels).
/// @param capacity la capacité maximale d'objets UI que le système peut gérer.
/// @return Le système UI créé.
UISystem* UISystem_create(
    AssetManager* assets, AudioSystem* audioSystem,
    Viewport* viewport, float virtualWidth,
    int capacity);

/// @brief Détruit un système UI.
/// @param self le système UI.
void UISystem_destroy(UISystem* self);

void UISystem_update(UISystem* self);

void UISystem_render(UISystem* self, GraphicsSystem* graphicsSystem);
void UISystem_drawGizmos(UISystem* self, GizmosSystem* gizmosSystem);


INLINE void UISystem_setAudioGroupIndex(UISystem* self, int audioGroupIndex)
{
    assert(self && "self must not be NULL");
    self->m_audioGroupIndex = audioGroupIndex;
}

/// @brief Convertit une coordonnée X du viewport vers le système UI virtuel.
/// @param self le système UI.
/// @param x la coordonnée X dans le viewport (en pixels écran).
/// @return La coordonnée X dans le système UI virtuel (0-640).
float UISystem_viewToUIX(const UISystem* self, float x);

/// @brief Convertit une coordonnée Y du viewport vers le système UI virtuel.
/// @param self le système UI.
/// @param y la coordonnée Y dans le viewport (en pixels écran).
/// @return La coordonnée Y dans le système UI virtuel (0-360).
float UISystem_viewToUIY(const UISystem* self, float y);

float UISystem_viewToUILength(const UISystem* self, float length);

void UISystem_uiToView(const UISystem* self, Vec2 uiPosition, float* x, float* y);
Vec2 UISystem_uiToViewV(const UISystem* self, Vec2 uiPosition);

/// @brief Convertit une coordonnée X du système UI virtuel vers le viewport.
/// @param self le système UI.
/// @param positionX la coordonnée X dans le système UI virtuel (0-640).
/// @return La coordonnée X dans le viewport (en pixels écran).
float UISystem_uiToViewX(const UISystem* self, float positionX);

/// @brief Convertit une coordonnée Y du système UI virtuel vers le viewport.
/// @param self le système UI.
/// @param positionY la coordonnée Y dans le système UI virtuel (0-360).
/// @return La coordonnée Y dans le viewport (en pixels écran).
float UISystem_uiToViewY(const UISystem* self, float positionY);

float UISystem_uiToViewLength(const UISystem* self, float length);

INLINE UIObject* UISystem_getCanvas(const UISystem* self)
{
    assert(self && "self must not be NULL");
    return self->m_canvas;
}

bool UISystem_hasObjectId(UISystem* self, UIObjectId id);
bool UISystem_hasObject(UISystem* self, UIObject* object);
UIObject* UISystem_getObjectById(UISystem* self, UIObjectId id);

void UISystem_addObject(UISystem* self, UIObject* object);
void UISystem_removeObjectId(UISystem* self, UIObjectId id);


void UISystem_playSFX(UISystem* self, int audioId);
