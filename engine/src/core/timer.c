/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/timer.h"

Timer* Timer_create()
{
    Timer* self = (Timer*)calloc(1, sizeof(Timer));
    ASSERT_NEW(self);

    self->currentTime = 0;
    self->previousTime = 0;

    self->delta = 0;
    self->unscaledDelta = 0;
    self->elapsed = 0;
    self->unscaledElapsed = 0;

    self->maxDelta = 100;
    self->scale = 1.0f;

    return self;
}

void Timer_destroy(Timer* self)
{
    if (!self) return;
    free(self);
}

void Timer_start(Timer* self)
{
    assert(self && "self must not be NULL");
    self->currentTime = SDL_GetTicks();
    self->previousTime = SDL_GetTicks();
    self->delta = 0;
}

void Timer_update(Timer* self)
{
    assert(self && "self must not be NULL");
    self->previousTime = self->currentTime;
    self->currentTime = SDL_GetTicks();

    self->unscaledDelta = self->currentTime - self->previousTime;
    if (self->unscaledDelta > self->maxDelta)
    {
        self->unscaledDelta = self->maxDelta;
    }
    self->delta = (Uint64)(self->scale * (double)self->unscaledDelta);

    self->unscaledElapsed += self->unscaledDelta;
    self->elapsed += self->delta;
}
