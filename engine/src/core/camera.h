/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/viewport.h"

/// @brief Structure représentant une caméra.
/// Gère uniquement la conversion entre coordonnées monde et coordonnées viewport.
typedef struct Camera
{
    /// @brief Rectangle représentant la vue de la caméra.
    /// Ces coordonnées sont dans le référentiel monde.
    AABB m_worldView;

    /// @brief Pointeur vers le viewport utilisé (non possédé).
    Viewport* m_viewport;

    Vec2 m_scale;
} Camera;

/// @brief Crée une nouvelle caméra.
/// @param viewport le viewport à utiliser (non possédé par la caméra).
/// @return La caméra créée.
Camera* Camera_create(Viewport* viewport);

/// @brief Détruit une caméra.
/// @param self la caméra.
void Camera_destroy(Camera* self);

void Camera_update(Camera* self);

/// @brief Renvoie le rectangle vu par la caméra.
/// Les coordonnées sont exprimées dans le référentiel monde.
/// @param self la caméra.
/// @return Le rectangle vu par la caméra.
INLINE AABB Camera_getWorldView(const Camera* self)
{
    assert(self && "The Camera must be created");
    return self->m_worldView;
}

/// @brief Définit le rectangle vu par la caméra.
/// Les coordonnées sont exprimées dans le référentiel monde.
/// @param self la caméra.
/// @param worldView le rectangle.
void Camera_setWorldView(Camera* self, AABB worldView);

/// @brief Déplace le rectangle vu par la caméra.
/// Les coordonnées sont exprimées dans le référentiel monde.
/// @param self la caméra.
/// @param displacement le vecteur de translation (dans le référentiel monde).
void Camera_translateWorldView(Camera* self, Vec2 displacement);

/// @brief Renvoie le facteur d'échelle par lequel il faut multiplier des
/// distances exprimées dans le reférentiel monde pour obtenir des distances
/// exprimées dans le référentiel vue (en pixels).
/// @param self la caméra.
/// @return Le facteur d'échelle monde vers vue (en pixels).
float Camera_getWorldToViewScale(const Camera* self);

/// @brief Transforme des coordonnées exprimées dans le référentiel monde vers
/// le référentiel vue (en pixels).
/// @param[in] self la caméra.
/// @param[in] position la position d'un point dans le référentiel monde.
/// @param[out] x l'abscisse du point dans la vue (en pixels).
/// @param[out] y l'ordonnée du point dans la vue (en pixels).
void Camera_worldToView(const Camera* self, Vec2 position, float* x, float* y);

float Camera_worldToViewX(const Camera* self, float positionX);
float Camera_worldToViewY(const Camera* self, float positionY);
Vec2 Camera_worldToViewV(const Camera* self, Vec2 vec);
float Camera_worldToViewLength(const Camera* self, float length);

/// @brief Transforme des coordonnées exprimée dans le référentiel de la vue
/// (en pixels) vers le référentiel monde.
/// @param[in] self camera la caméra.
/// @param[in] x l'abscisse d'un point dans le référentiel vue (en pixels).
/// @param[in] y l'ordonnée d'un point dans le référentiel vue (en pixels).
/// @param[out] position la position du point dans le référentiel monde.
void Camera_viewToWorld(const Camera* self, float x, float y, Vec2* position);

