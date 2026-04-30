/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/anim/easing_fct.h"
#include "core/anim/anim_common.h"

typedef struct BasicAnimEvent
{
    AnimEventType type;
    int index;
} BasicAnimEvent;

typedef struct BasicAnimState
{
    float speed;

    /// @brief Les flags de l'animation.
    int flags;

    /// @brief Accumulateur.
    float accu;

    /// @brief Indice du cycle courant. 
    int cycleIdx;

    float cycleTime;

    /// @brief Nombre de cycles de l'animation.
    /// L'animation tourne en boucle si ce membre vaut -1.
    int cycleCount;

    bool saveEvents;
    int eventCount;
    BasicAnimEvent events[MAX_ANIM_EVENT_COUNT];
} BasicAnimState;


void BasicAnimState_init(BasicAnimState* self, float cycleTime, int cycleCount, bool saveEvents);
float BasicAnimState_getProgress(const BasicAnimState* self);
void BasicAnimState_play(BasicAnimState* self);
void BasicAnimState_playWithDelay(BasicAnimState* self, float delay);
void BasicAnimState_resume(BasicAnimState* self);
void BasicAnimState_stop(BasicAnimState* self);
void BasicAnimState_update(BasicAnimState* self, float dt);

void BasicAnimState_setReversed(BasicAnimState* self, bool reversed);
void BasicAnimState_setAlternate(BasicAnimState* self, bool alternate);

INLINE bool BasicAnimState_isStopped(const BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & ANIM_FLAG_STOPPED) != 0;
}

INLINE bool BasicAnimState_isDelayed(const BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    const float epsilon = 0.001f;
    return self->accu < -epsilon;
}

INLINE bool BasicAnimState_isPlaying(const BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & (ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED)) == 0;
}

typedef struct LerpValuesFloat
{
    float value0;
    float value1;
    EasingFct easing;
} LerpValuesFloat;

void LerpValuesFloat_init(LerpValuesFloat* self, float value0, float value1, EasingFct easing);

float BasicAnimState_getValueFloat(const BasicAnimState* self, const LerpValuesFloat* lerpValues);

