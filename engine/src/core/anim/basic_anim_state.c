/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/anim/basic_anim_state.h"

static void BasicAnimState_emplaceBackEvent(BasicAnimState* self, AnimEventType type, int index)
{
    if (self->saveEvents == false || self->eventCount >= MAX_ANIM_EVENT_COUNT) return;
    BasicAnimEvent event = { .type = type, .index = index };
    self->events[self->eventCount++] = event;
}

void BasicAnimState_init(BasicAnimState* self, float cycleTime, int cycleCount, bool saveEvents)
{
    assert(self && "self must not be NULL");
    self->accu = 0.f;
    self->cycleIdx = 0;
    self->cycleTime = cycleTime;
    self->cycleCount = cycleCount;
    self->speed = 1.f;
    self->flags = ANIM_FLAG_NONE;
    self->saveEvents = saveEvents;
    self->eventCount = 0;
}

float BasicAnimState_getProgress(const BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    float t = self->accu / self->cycleTime;
    if (t > 1.f) t -= floorf(t);

    // Animation alternée
    if (self->flags & ANIM_FLAG_ALTERNATE)
    {
        t = (t < 0.5f) ? 2.f * t : 2.f - 2.f * t;
    }

    // Animation inversée
    if (self->flags & ANIM_FLAG_REVERSED)
    {
        t = 1.f - t;
    }

    return t;
}

void BasicAnimState_setReversed(BasicAnimState* self, bool reversed)
{
    assert(self && "self must not be NULL");
    if (reversed)
    {
        self->flags |= ANIM_FLAG_REVERSED;
    }
    else
    {
        self->flags &= (~ANIM_FLAG_REVERSED);
    }
}

void BasicAnimState_setAlternate(BasicAnimState* self, bool alternate)
{
    assert(self && "self must not be NULL");
    if (alternate)
    {
        self->flags |= ANIM_FLAG_ALTERNATE;
    }
    else
    {
        self->flags &= (~ANIM_FLAG_ALTERNATE);
    }
}

void BasicAnimState_play(BasicAnimState* self)
{
    BasicAnimState_playWithDelay(self, 0.f);
}

void BasicAnimState_playWithDelay(BasicAnimState* self, float delay)
{
    assert(self && "self must not be NULL");
    self->accu = -delay;
    self->cycleIdx = 0;
    self->flags &= ~(ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED);
}

void BasicAnimState_resume(BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    self->flags &= ~(ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED);
}

void BasicAnimState_stop(BasicAnimState* self)
{
    assert(self && "self must not be NULL");
    self->flags |= ANIM_FLAG_STOPPED;
}

void BasicAnimState_update(BasicAnimState* self, float dt)
{
    assert(self && "self must not be NULL");
    self->eventCount = 0;

    if (BasicAnimState_isPlaying(self) == false) return;

    const float epsilon = 0.001f;
    const float prevAccu = self->accu;

    self->accu += self->speed * dt;

    if (prevAccu < 0.f && self->accu >= 0.f)
    {
        BasicAnimState_emplaceBackEvent(self, ANIM_EVENT_ANIMATION_START, 0);
    }

    while (self->accu >= self->cycleTime - epsilon)
    {
        BasicAnimState_emplaceBackEvent(self, ANIM_EVENT_CYCLE_END, self->cycleIdx);

        self->accu -= self->cycleTime;
        self->cycleIdx++;

        if ((self->cycleCount > 0) && (self->cycleIdx >= self->cycleCount))
        {
            // Fin de l'animation
            BasicAnimState_emplaceBackEvent(self, ANIM_EVENT_ANIMATION_END, self->cycleIdx);

            self->accu = self->cycleTime;
            self->flags |= ANIM_FLAG_PAUSED;

            if (self->flags & ANIM_FLAG_STOP_AT_END)
            {
                self->flags |= ANIM_FLAG_STOPPED;
            }
            return;
        }
    }
}

void LerpValuesFloat_init(LerpValuesFloat* self, float value0, float value1, EasingFct easing)
{
    assert(self && "self must not be NULL");
    self->value0 = value0;
    self->value1 = value1;
    self->easing = easing;
}

float BasicAnimState_getValueFloat(const BasicAnimState* self, const LerpValuesFloat* lerpValues)
{
    assert(self && "self must not be NULL");
    float p = BasicAnimState_getProgress(self);
    if (lerpValues->easing) p = lerpValues->easing(p);
    p = fmaxf(0.f, fminf(p, 1.f));
    return (1.f - p) * lerpValues->value0 + p * lerpValues->value1;
}
