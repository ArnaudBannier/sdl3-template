/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"
#include "core/camera.h"
#include "core/anim/sprite_anim.h"
#include "core/anim/sprite_anim_manager.h"

typedef enum TransformSpace
{
    TRANSFORM_SPACE_WORLD,
    TRANSFORM_SPACE_UI,
} TransformSpace;

typedef struct FixedUpdateTransform
{
    TransformSpace space;
    Vec2 currPosition;
    Vec2 prevPosition;
    float currAngle;
    float prevAngle;
} FixedUpdateTransform;

void FixedUpdateTransform_set(
    FixedUpdateTransform* self,
    TransformSpace space,
    Vec2 position,
    float angle
);

typedef struct Transform
{
    TransformSpace space;
    Vec2 position;
    float angle;
} Transform;

void Transform_setFromFixedUpdate(
    Transform* self,
    const FixedUpdateTransform* fixedUpdate,
    float alpha
);

typedef struct RenderSortingLayer
{
    int layer;
    int orderInLayer;
} RenderSortingLayer;

typedef struct RenderBlendMod
{
    SDL_BlendMode mode;
    float alpha;
} RenderBlendMod;

void RenderBlendMod_init(RenderBlendMod* self);
void RenderBlendMod_setAlpha(RenderBlendMod* self, float alpha);

typedef struct RenderColorMod
{
    float r, g, b;
} RenderColorMod;

void RenderColorMod_init(RenderColorMod* self);
void RenderColorMod_setColor(RenderColorMod* self, SDL_Color color);

typedef struct RenderAnchor
{
    Vec2 value;
} RenderAnchor;

typedef struct RenderFlip
{
    SDL_FlipMode mode;
} RenderFlip;

typedef struct RenderTexture
{
    SDL_Texture* texture;
    TextureRegion* region;
} RenderTexture;

void RenderTexture_setFromGroup(
    RenderTexture* self,
    SpriteGroup* spriteGroup,
    int spriteIdx
);

void RenderTexture_setFromAnim(
    RenderTexture* self,
    SpriteAnimManager* animManager,
    SpriteAnimState* animState
);

typedef struct RenderRect
{
    SDL_Color color;
    bool filled;
} RenderRect;

typedef struct RenderText
{
    TTF_Text* ttfText;
    SDL_Color color;
    Vec2 anchor;
} RenderText;

typedef enum RenderDimSpace
{
    RENDER_DIM_SPACE_WORLD,
    RENDER_DIM_SPACE_WORLD_PX_PER_UNITS,
    RENDER_DIM_SPACE_UI,
} RenderDimSpace;

typedef struct RenderDim
{
    RenderDimSpace space;
    float width;
    float height;
} RenderDim;

void RenderDim_setPixelsPerUnit(
    RenderDim* self,
    float pixelsPerUnit
);

void RenderDim_setWorldDimensions(
    RenderDim* self,
    float width,
    float height
);

void RenderDim_setUIDimensions(
    RenderDim* self,
    float width,
    float height
);
