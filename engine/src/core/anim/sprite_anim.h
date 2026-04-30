/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/anim/anim_common.h"
#include "core/anim/sprite_anim_state.h"
#include "core/sprite_sheet.h"

/// @brief Structure représentant une animation.
typedef struct SpriteAnim
{
    AnimId m_id;

    SpriteGroup* m_spriteGroup;

    /// @brief Courbe d'accélération de l'animation.
    /// @brief La durée d'un cycle.
    float m_cycleTime;

    /// @brief Nombre de cycles de l'animation.
    /// L'animation tourne en boucle si ce membre vaut -1.
    int m_cycleCount;

    int m_frameCount;

    float* m_keyFrames;
} SpriteAnim;

SpriteAnim* SpriteAnim_create(AnimId id, SpriteGroup* spriteGroup);
void SpriteAnim_destroy(SpriteAnim* self);

INLINE AnimId SpriteAnim_getId(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_id;
}

INLINE SpriteGroup* SpriteAnim_getSpriteGroup(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_spriteGroup;
}

INLINE int SpriteAnim_getFrameCount(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_frameCount;
}

INLINE int SpriteAnim_getCycleCount(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_cycleCount;
}

INLINE float SpriteAnim_getCycleTime(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_cycleTime;
}

INLINE float SpriteAnim_getTotalTime(const SpriteAnim* self)
{
    assert(self && "self must not be NULL");
    return self->m_cycleCount >= 0 ? self->m_cycleTime * (float)(self->m_cycleCount) : self->m_cycleTime;
}

INLINE void SpriteAnim_setCycleCount(SpriteAnim* self, int cycleCount)
{
    assert(self && "self must not be NULL");
    self->m_cycleCount = cycleCount;
}

INLINE void SpriteAnim_setCycleTime(SpriteAnim* self, float cycleTime)
{
    assert(self && "self must not be NULL");
    self->m_cycleTime = cycleTime;
}

INLINE void SpriteAnim_setFPS(SpriteAnim* self, float fps)
{
    assert(self && "self must not be NULL");
    const int frameCount = SpriteGroup_getSpriteCount(self->m_spriteGroup);
    self->m_cycleTime = (float)frameCount / fps;
}

INLINE float SpriteAnim_getStateProgress(const SpriteAnim* self, const SpriteAnimState* state)
{
    assert(self && "self must not be NULL");
    assert(state && "state must not be NULL");
    return state->accu / self->m_cycleTime;
}

void SpriteAnim_updateState(
    const SpriteAnim* self,
    SpriteAnimState* state,
    float dt);
