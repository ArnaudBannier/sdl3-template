/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/anim/anim_common.h"

typedef struct SpriteAnimEvent
{
    AnimEventType type;
    int index;
    AnimId id;
} SpriteAnimEvent;

typedef struct SpriteAnimState
{
    AnimId id;

    /// @brief Le coefficient multiplicateur de la vitesse de lecture.
    float speed;

    /// @brief Les flags de l'animation.
    /// Les valeurs possibles des flags sont données dans l'énumération AnimFlags.
    AnimFlag flags;

    /// @brief Accumulateur.
    float accu;

    /// @brief Indice du cycle courant. 
    int cycleIdx;

    int frameIdx;

    bool saveEvents;
    int eventCount;
    SpriteAnimEvent events[MAX_ANIM_EVENT_COUNT];
} SpriteAnimState;

void SpriteAnimState_init(SpriteAnimState* self, AnimId id);
void SpriteAnimState_play(SpriteAnimState* self);
void SpriteAnimState_playWithDelay(SpriteAnimState* self, float delay);
void SpriteAnimState_pause(SpriteAnimState* self);
void SpriteAnimState_resume(SpriteAnimState* self);
void SpriteAnimState_stop(SpriteAnimState* self);

INLINE void SpriteAnimState_setAnimation(SpriteAnimState* self, AnimId id)
{
    assert(self && "self must not be NULL");
    self->id = id;
}

INLINE bool SpriteAnimState_isStopped(const SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & ANIM_FLAG_STOPPED) != 0;
}

INLINE bool SpriteAnimState_isDelayed(const SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    const float epsilon = 0.001f;
    return self->accu < -epsilon;
}

INLINE bool SpriteAnimState_isPlaying(const SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & (ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED)) == 0;
}
