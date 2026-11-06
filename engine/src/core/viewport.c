/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/viewport.h"

Viewport* Viewport_create()
{
    Viewport* self = (Viewport*)calloc(1, sizeof(Viewport));
    ASSERT_NEW(self);

    self->rect = (SDL_FRect){ 0.0f, 0.0f, 640.0f, 360.0f };

    return self;
}

void Viewport_destroy(Viewport* self)
{
    if (!self) return;
    free(self);
}

void Viewport_updateMain(Viewport* self, SDL_Renderer* renderer, float aspectRatio)
{
    assert(self && "self must not be NULL");
    assert(renderer && "renderer must not be NULL");

    int outW = 0, outH = 0;
    bool success = SDL_GetRenderOutputSize(renderer, &outW, &outH);
    assert(success);

    const float outputAspectRatio = (float)outW / (float)outH;

    self->rect.x = 0.f;
    self->rect.y = 0.f;

    if (outputAspectRatio >= aspectRatio)
    {
        // Output plus large
        self->rect.w = aspectRatio * (float)outH;
        self->rect.h = (float)outH;
        self->rect.x = 0.5f * (outW - self->rect.w);
    }
    else
    {
        // Output plus haut
        self->rect.w = (float)outW;
        self->rect.h = (float)outW / aspectRatio;
        self->rect.y = 0.5f * (outH - self->rect.h);
    }

    self->rect.x = roundf(self->rect.x);
    self->rect.y = roundf(self->rect.y);
    self->rect.w = roundf(self->rect.w);
    self->rect.h = roundf(self->rect.h);
}

void Viewport_apply(Viewport* self, SDL_Renderer* renderer)
{
    assert(self && "self must not be NULL");
    assert(renderer && "renderer must not be NULL");

    SDL_Rect viewport = { 0 };
    viewport.x = (int)(self->rect.x);
    viewport.y = (int)(self->rect.y);
    viewport.w = (int)(self->rect.w);
    viewport.h = (int)(self->rect.h);
    SDL_SetRenderViewport(renderer, &viewport);
}
