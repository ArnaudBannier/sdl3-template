/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "utils/utils.h"

bool RenderTexture(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor
);

bool RenderTextureRotated(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor,
    const double angle, SDL_FlipMode flip
);

bool RenderTexture9Grid(
    SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_FRect* srcRect, const SDL_FRect* dstRect, Vec2 anchor,
    float l, float r, float t, float b, float scale
);
