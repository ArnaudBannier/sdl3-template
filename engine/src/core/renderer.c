/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/renderer.h"

bool RenderTexture(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor)
{
    if (dstRect == NULL)
    {
        return SDL_RenderTexture(renderer, texture, srcRect, NULL);
    }

    SDL_FRect dst = *dstRect;
    dst.x -= anchor.x * dst.w;
    dst.y -= (1.f - anchor.y) * dst.h;
    return SDL_RenderTexture(renderer, texture, srcRect, &dst);
}

bool RenderTextureRotated(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor,
    const double angle, SDL_FlipMode flip)
{
    if (dstRect == NULL)
    {
        return SDL_RenderTextureRotated(renderer, texture, srcRect, NULL, angle, NULL, flip);
    }
    SDL_FPoint center = { 0 };
    center.x = anchor.x * dstRect->w;
    center.y = (1.f - anchor.y) * dstRect->h;

    SDL_FRect dst = *dstRect;
    dst.x -= anchor.x * dst.w;
    dst.y -= (1.f - anchor.y) * dst.h;
    return SDL_RenderTextureRotated(renderer, texture, srcRect, &dst, angle, &center, flip);
}

bool RenderTexture9Grid(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor,
    float l, float r, float t, float b, float scale)
{
    scale = SDL_roundf(scale);
    if (dstRect == NULL)
    {
        return SDL_RenderTexture9Grid(renderer, texture, srcRect, l, r, t, b, scale, NULL);
    }

    SDL_FRect dst = *dstRect;
    dst.x -= anchor.x * dst.w;
    dst.y -= (1.f - anchor.y) * dst.h;
    return SDL_RenderTexture9Grid(renderer, texture, srcRect, l, r, t, b, scale, &dst);
}
