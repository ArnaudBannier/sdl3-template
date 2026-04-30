/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/components.h"
#include "ui/ui_system.h"

void RenderDim_setPixelsPerUnit(RenderDim* self, float pixelsPerUnit)
{
    self->space = RENDER_DIM_SPACE_WORLD_PX_PER_UNITS;
    self->width = pixelsPerUnit;
    self->height = pixelsPerUnit;
}

void RenderDim_setWorldDimensions(RenderDim* self, float width, float height)
{
    self->space = RENDER_DIM_SPACE_WORLD;
    self->width = width;
    self->height = height;
}

void RenderDim_setUIDimensions(RenderDim* self, float width, float height)
{
    self->space = RENDER_DIM_SPACE_UI;
    self->width = width;
    self->height = height;
}

void FixedUpdateTransform_set(
    FixedUpdateTransform* self,
    TransformSpace space, Vec2 position, float angle)
{
    assert(self && "self must not be NULL");
    self->space = space;
    self->currPosition = position;
    self->prevPosition = position;
    self->currAngle = angle;
    self->prevAngle = angle;
}

void Transform_setFromFixedUpdate(
    Transform* self,
    const FixedUpdateTransform* fixedUpdate,
    float alpha)
{
    assert(self && "self must not be NULL");
    assert(fixedUpdate && "fixedUpdate must not be NULL");
    self->space = fixedUpdate->space;
    self->position = Vec2_lerp(
        fixedUpdate->prevPosition,
        fixedUpdate->currPosition,
        alpha
    );
    self->angle = Float_lerp(
        fixedUpdate->prevAngle,
        fixedUpdate->currAngle,
        alpha
    );
}

void RenderBlendMod_init(RenderBlendMod* self)
{
    self->mode = SDL_BLENDMODE_BLEND;
    self->alpha = 1.f;
}

void RenderBlendMod_setAlpha(RenderBlendMod* self, float alpha)
{
    self->mode = SDL_BLENDMODE_BLEND;
    self->alpha = alpha;
}

void RenderColorMod_init(RenderColorMod* self)
{
    self->r = 1.f;
    self->g = 1.f;
    self->b = 1.f;
}

void RenderColorMod_setColor(RenderColorMod* self, SDL_Color color)
{
    self->r = color.r / 255.f;
    self->g = color.g / 255.f;
    self->b = color.b / 255.f;
}

void RenderTexture_setFromGroup(RenderTexture* self, SpriteGroup* spriteGroup, int spriteIdx)
{
    assert(self && "self must not be NULL");
    assert(spriteGroup && "spriteGroup must not be NULL");
    self->texture = SpriteGroup_getTexture(spriteGroup);
    self->region = SpriteGroup_getTextureRegion(spriteGroup, spriteIdx);
}

void RenderTexture_setFromAnim(RenderTexture* self, SpriteAnimManager* animManager, SpriteAnimState* animState)
{
    assert(self && "self must not be NULL");
    assert(animManager && "animManager must not be NULL");
    assert(animState && "animState must not be NULL");
    SpriteAnim* spriteAnim = SpriteAnimManager_getSpriteAnim(animManager, animState->id);
    if (spriteAnim == NULL || SpriteAnimState_isDelayed(animState))
    {
        return;
    }
    SpriteGroup* spriteGroup = SpriteAnim_getSpriteGroup(spriteAnim);
    self->texture = SpriteGroup_getTexture(spriteGroup);
    self->region = SpriteGroup_getTextureRegion(spriteGroup, animState->frameIdx);
}
