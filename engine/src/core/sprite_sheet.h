/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"

typedef struct SpriteSheet SpriteSheet;
typedef struct AssetManager AssetManager;

typedef struct TextureRegion
{
    SDL_FRect srcRect;
    bool hasBorders;
    float left, right, top, bottom;
    float scale;
} TextureRegion;

typedef struct SpriteGroup
{
    SpriteSheet* spriteSheet;
    char* name;
    int* spriteIndices;
    int spriteCount;
} SpriteGroup;

SpriteGroup* SpriteGroup_create(SpriteSheet* spriteSheet);
void SpriteGroup_destroy(SpriteGroup* self);

INLINE int SpriteGroup_getSpriteCount(SpriteGroup* self)
{
    assert(self && "self must not be NULL");
    return self->spriteCount;
}

const SDL_FRect* SpriteGroup_getSrcRect(SpriteGroup* self, int index);
TextureRegion* SpriteGroup_getTextureRegion(SpriteGroup* self, int index);
float SpriteGroup_getAspectRatio(SpriteGroup* self, int index);
SDL_Texture* SpriteGroup_getTexture(SpriteGroup* self);

/// @brief Structure représentant un atlas de textures.
typedef struct SpriteSheet
{
    AssetManager* assetManager;
    int sheetId;
    SDL_Texture* cachedTexture;

    SpriteGroup** groups;
    int groupCount;

    TextureRegion* sprites;
    int spriteCount;

    bool pixelArt;
} SpriteSheet;

SpriteSheet* SpriteSheet_create(AssetManager* assetManager, int sheetId, const char* descBuffer, Uint64 descLength);
void SpriteSheet_destroy(SpriteSheet* self);

SpriteGroup* SpriteSheet_getGroupByName(SpriteSheet* self, const char* name);
SpriteGroup* SpriteSheet_getGroupByIndex(SpriteSheet* self, int index);
SDL_Texture* SpriteSheet_getTexture(SpriteSheet* self);

INLINE bool SpriteSheet_isPixelArt(const SpriteSheet* self)
{
    assert(self && "self must not be NULL");
    return self->pixelArt;
}
