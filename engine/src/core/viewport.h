/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"

/// @brief Structure représentant un viewport (zone d'affichage).
/// Gère le ratio d'aspect et les conversions entre coordonnées écran et viewport.
typedef struct Viewport
{
    /// @brief Rectangle du viewport en pixels écran.
    SDL_FRect m_rect;

} Viewport;

/// @brief Crée un nouveau viewport avec un ratio d'aspect donné.
/// @return Le viewport créé.
Viewport* Viewport_create();

/// @brief Détruit un viewport.
/// @param self le viewport.
void Viewport_destroy(Viewport* self);

/// @brief Met à jour le viewport en fonction de la taille de sortie du renderer.
/// Calcule le rectangle optimal pour respecter le ratio d'aspect.
/// @param self le viewport.
/// @param renderer le renderer SDL.
/// @param aspectRatio le ratio d'aspect (par exemple 16.0f/9.0f).
void Viewport_updateMain(Viewport* self, SDL_Renderer* renderer, float aspectRatio);

/// @brief Applique le viewport au renderer SDL.
/// @param self le viewport.
/// @param renderer le renderer SDL.
void Viewport_apply(Viewport* self, SDL_Renderer* renderer);

/// @brief Renvoie le rectangle du viewport en pixels écran.
/// @param self le viewport.
/// @return Le rectangle du viewport.
INLINE SDL_FRect Viewport_getRect(const Viewport* self)
{
    assert(self && "Viewport must not be NULL");
    return self->m_rect;
}

/// @brief Renvoie la largeur du viewport en pixels.
/// @param self le viewport.
/// @return La largeur du viewport.
INLINE float Viewport_getWidth(const Viewport* self)
{
    assert(self && "Viewport must not be NULL");
    return self->m_rect.w;
}

/// @brief Renvoie la hauteur du viewport en pixels.
/// @param self le viewport.
/// @return La hauteur du viewport.
INLINE float Viewport_getHeight(const Viewport* self)
{
    assert(self && "Viewport must not be NULL");
    return self->m_rect.h;
}

/// @brief Renvoie le ratio d'aspect (largeur / hauteur) du viewport.
/// @param self le viewport.
/// @return Le ratio d'aspect du viewport.
INLINE float Viewport_getAspectRatio(const Viewport* self)
{
    assert(self && "Viewport must not be NULL");
    return self->m_rect.w / self->m_rect.h;
}
