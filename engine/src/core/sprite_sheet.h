/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "utils/utils.h"

typedef struct SpriteSheet SpriteSheet;

typedef struct Sprite
{
    SDL_FRect srcRect;
    bool hasBorders;
    float left, right, top, bottom;
    float scale;
} Sprite;

typedef struct SpriteGroup
{
    SpriteSheet* m_spriteSheet;
    char* m_name;
    int* m_spriteIndices;
    int m_spriteCount;
} SpriteGroup;

SpriteGroup* SpriteGroup_create(SpriteSheet* spriteSheet);
void SpriteGroup_destroy(SpriteGroup* self);

void SpriteGroup_render(
    SpriteGroup* self, int index,
    const SDL_FRect* dstRect, Vec2 anchor,
    float scale);

void SpriteGroup_renderRotated(
    SpriteGroup* self, int index,
    const SDL_FRect* dstRect, Vec2 anchor,
    const double angle, const SDL_FPoint* center, const SDL_FlipMode flip);

void SpriteGroup_setOpacity(SpriteGroup* self, Uint8 alpha);
void SpriteGroup_setOpacityFloat(SpriteGroup* self, float alpha);
void SpriteGroup_setColorModFloat(SpriteGroup* self, float r, float g, float b);
float SpriteGroup_getAspectRatio(SpriteGroup* self, int index);

/// @brief Structure représentant un atlas de textures.
typedef struct SpriteSheet
{
    SDL_Texture* m_texture;
    SpriteGroup** m_groups;
    int m_groupCount;

    Sprite* m_sprites;
    int m_spriteCount;

    bool m_pixelArt;
} SpriteSheet;

SpriteSheet* SpriteSheet_create(SDL_Texture* texture, const char* desc, Uint64 descLength);
void SpriteSheet_destroy(SpriteSheet* self);

SpriteGroup* SpriteSheet_getGroupByName(SpriteSheet* self, const char* name);
SpriteGroup* SpriteSheet_getGroupByIndex(SpriteSheet* self, int index);

