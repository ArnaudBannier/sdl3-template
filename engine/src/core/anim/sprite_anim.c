/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/anim/sprite_anim.h"


static void SpriteAnimState_emplaceBackEvent(SpriteAnimState* self, AnimEventType type, int index, AnimId animId)
{
    if (self->saveEvents == false || self->eventCount >= MAX_ANIM_EVENT_COUNT) return;
    SpriteAnimEvent event = { .type = type, .index = index, .id = animId };
    self->events[self->eventCount++] = event;
}

SpriteAnim* SpriteAnim_create(AnimId id, SpriteGroup* spriteGroup)
{
    SpriteAnim* self = (SpriteAnim*)calloc(1, sizeof(SpriteAnim));
    ASSERT_NEW(self);
    self->id = id;
    self->spriteGroup = spriteGroup;
    self->cycleTime = 1.f;
    self->cycleCount = -1;

    const int frameCount = SpriteGroup_getSpriteCount(spriteGroup);
    self->frameCount = frameCount;
    self->keyFrames = (float*)calloc(frameCount, sizeof(float));
    ASSERT_NEW(self->keyFrames);
    for (int i = 0; i < frameCount; i++)
    {
        self->keyFrames[i] = (float)i / (float)frameCount;
    }

    return self;
}

void SpriteAnim_destroy(SpriteAnim* self)
{
    if (!self) return;
    free(self->keyFrames);
    free(self);
}

void SpriteAnim_updateState(const SpriteAnim* self, SpriteAnimState* state, float dt)
{
    assert(self && "self must not be NULL");
    state->eventCount = 0;

    if (SpriteAnimState_isPlaying(state) == false) return;
    if (dt <= 0.f) return;

    const AnimId id = self->id;
    const float cycleTime = self->cycleTime;
    const int cycleCount = self->cycleCount;
    const float* keyFrames = self->keyFrames;
    const int frameCount = self->frameCount;
    const float epsilon = 0.001f;
    const float prevAccu = state->accu;
    float prevProgression = SpriteAnim_getStateProgress(self, state);

    state->accu += state->speed * dt;

    if (prevAccu <= 0.f && state->cycleIdx == 0)
    {
        SpriteAnimState_emplaceBackEvent(state, ANIM_EVENT_ANIMATION_START, 0, id);
    }

    while (state->accu >= cycleTime - epsilon)
    {
        for (int i = 0; i < frameCount; i++)
        {
            if (keyFrames[i] > prevProgression - epsilon)
            {
                SpriteAnimState_emplaceBackEvent(state, ANIM_EVENT_FRAME_CHANGED, i, id);
            }
        }
        prevProgression = 0.f;

        SpriteAnimState_emplaceBackEvent(state, ANIM_EVENT_CYCLE_END, state->cycleIdx, id);

        state->accu -= cycleTime;
        state->cycleIdx++;

        if ((cycleCount > 0) && (state->cycleIdx >= cycleCount))
        {
            // Fin de l'animation
            SpriteAnimState_emplaceBackEvent(state, ANIM_EVENT_ANIMATION_END, 0, id);

            state->accu = cycleTime;
            state->frameIdx = frameCount - 1;
            state->flags |= ANIM_FLAG_PAUSED;

            if (state->flags & ANIM_FLAG_STOP_AT_END)
            {
                state->flags |= ANIM_FLAG_STOPPED;
            }
            return;
        }
    }

    const float currProgression = SpriteAnim_getStateProgress(self, state);
    for (int i = 0; i < frameCount; i++)
    {
        if (keyFrames[i] > prevProgression - epsilon &&
            keyFrames[i] <= currProgression - epsilon)
        {
            SpriteAnimState_emplaceBackEvent(state, ANIM_EVENT_FRAME_CHANGED, i, id);
            state->frameIdx = i;
        }
    }
}
