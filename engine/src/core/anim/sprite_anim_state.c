/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/anim/sprite_anim_state.h"

void SpriteAnimState_init(SpriteAnimState* self, AnimId id)
{
    assert(self && "self must not be NULL");
    self->id = id;
    self->speed = 1.f;
    self->flags = ANIM_FLAG_NONE;
    self->accu = 0.f;
    self->cycleIdx = 0;
    self->frameIdx = 0;
    self->saveEvents = true;
    self->eventCount = 0;
}

void SpriteAnimState_play(SpriteAnimState* self)
{
    SpriteAnimState_playWithDelay(self, 0.f);
}

void SpriteAnimState_playWithDelay(SpriteAnimState* self, float delay)
{
    assert(self && "self must not be NULL");
    self->accu = -delay;
    self->cycleIdx = 0;
    self->frameIdx = 0;
    self->flags &= ~(ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED);
}

void SpriteAnimState_pause(SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    self->flags |= ANIM_FLAG_PAUSED;
}

void SpriteAnimState_resume(SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    self->flags &= ~(ANIM_FLAG_PAUSED | ANIM_FLAG_STOPPED);
}

void SpriteAnimState_stop(SpriteAnimState* self)
{
    assert(self && "self must not be NULL");
    self->cycleIdx = 0;
    self->frameIdx = 0;
    self->flags |= ANIM_FLAG_PAUSED;
}
