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

    self->m_rect = (SDL_FRect){ 0.0f, 0.0f, 640.0f, 360.0f };

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

    self->m_rect.x = 0.f;
    self->m_rect.y = 0.f;

    if (outputAspectRatio >= aspectRatio)
    {
        // Output plus large
        self->m_rect.w = aspectRatio * (float)outH;
        self->m_rect.h = (float)outH;
        self->m_rect.x = 0.5f * (outW - self->m_rect.w);
    }
    else
    {
        // Output plus haut
        self->m_rect.w = (float)outW;
        self->m_rect.h = (float)outW / aspectRatio;
        self->m_rect.y = 0.5f * (outH - self->m_rect.h);
    }

    self->m_rect.x = roundf(self->m_rect.x);
    self->m_rect.y = roundf(self->m_rect.y);
    self->m_rect.w = roundf(self->m_rect.w);
    self->m_rect.h = roundf(self->m_rect.h);
}

void Viewport_apply(Viewport* self, SDL_Renderer* renderer)
{
    assert(self && "self must not be NULL");
    assert(renderer && "renderer must not be NULL");

    SDL_Rect viewport = { 0 };
    viewport.x = (int)(self->m_rect.x);
    viewport.y = (int)(self->m_rect.y);
    viewport.w = (int)(self->m_rect.w);
    viewport.h = (int)(self->m_rect.h);
    SDL_SetRenderViewport(renderer, &viewport);
}
