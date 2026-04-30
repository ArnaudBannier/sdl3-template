/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/particle_system.h"
#include "engine_common.h"

void Particle_init(Particle* self, SpriteAnimManager* animManager, AnimId id)
{
    const SpriteAnim* spriteAnim = SpriteAnimManager_getSpriteAnim(animManager, id);
    const float lifetime = SpriteAnim_getTotalTime(spriteAnim);

    memset(self, 0, sizeof(Particle));
    SpriteAnimState_init(&(self->animState), id);
    BasicAnimState_init(&(self->alphaState), lifetime, 1, false);
    BasicAnimState_init(&(self->scaleState), lifetime, 1, false);
    LerpValuesFloat_init(&(self->alphaValues), 1.f, 1.f, EasingFct_Linear);
    LerpValuesFloat_init(&(self->scaleValues), 1.f, 1.f, EasingFct_Linear);

    RenderBlendMod_init(&self->blendMod);
    RenderDim_setPixelsPerUnit(&self->dimensions, 24.f);

    self->flip.mode = SDL_FLIP_NONE;
    self->lifetime = lifetime;
    self->remainingLifetime = lifetime;
    self->anchor.value = Vec2_anchor_center;

    self->transform.space = TRANSFORM_SPACE_WORLD;
    self->transform.position = Vec2_zero;
    self->transform.angle = 0.f;

    self->angularVelocity = 0.f;
    self->velocity = Vec2_zero;
    self->gravity = Vec2_zero;
    self->damping = Vec2_zero;
}

static bool Particle_update(Particle* self, SpriteAnimManager* animManager, float dt)
{
    SpriteAnimManager_updateState(animManager, &(self->animState), dt);
    BasicAnimState_update(&(self->alphaState), dt);
    BasicAnimState_update(&(self->scaleState), dt);

    if (SpriteAnimState_isDelayed(&(self->animState))) return true;

    self->remainingLifetime -= dt;

    if (self->remainingLifetime < 0.f)
        return false;

    self->velocity.x += dt * self->gravity.x;
    self->velocity.y += dt * self->gravity.y;
    self->velocity.x *= 1.0f / (1.0f + dt * self->damping.x);
    self->velocity.y *= 1.0f / (1.0f + dt * self->damping.y);

    self->transform.position.x += dt * self->velocity.x;
    self->transform.position.y += dt * self->velocity.y;
    self->transform.angle += dt * self->angularVelocity;

    return true;
}

static void Particle_render(Particle* self, GraphicsSystem* graphicsSystem, SpriteAnimManager* animManager)
{
    assert(self && "self must not be NULL");

    SpriteAnimState* animState = &(self->animState);
    SpriteAnim* spriteAnim = SpriteAnimManager_getSpriteAnim(animManager, animState->id);
    if (spriteAnim == NULL || SpriteAnimState_isDelayed(animState))
    {
        return;
    }

    RenderTexture texture = { 0 };
    RenderDim renderDim = self->dimensions;
    RenderBlendMod blendMod = self->blendMod;

    SpriteGroup* spriteGroup = SpriteAnim_getSpriteGroup(spriteAnim);
    texture.texture = SpriteGroup_getTexture(spriteGroup);
    texture.region = SpriteGroup_getTextureRegion(spriteGroup, animState->frameIdx);

    GraphicsCmd cmd = { 0 };
    cmd.transform = &(self->transform);
    cmd.sortingLayer = &(self->sortingLayer);
    cmd.anchor = &(self->anchor);
    cmd.texture = &texture;
    cmd.dimensions = &renderDim;
    cmd.blendMod = &blendMod;

    const float scale = BasicAnimState_getValueFloat(&(self->scaleState), &(self->scaleValues));
    renderDim.height *= scale;
    renderDim.width *= scale;

    const float alpha = BasicAnimState_getValueFloat(&(self->alphaState), &(self->alphaValues));
    blendMod.alpha *= alpha;

    RenderTexture_setFromAnim(&texture, animManager, &self->animState);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);

    //const float scale = BasicAnimState_getValueFloat(&(self->scaleState), &(self->scaleValues))
    //    * Camera_getWorldToViewScale(camera);
    //SDL_FRect dstRect = { 0 };
    //dstRect.x = Camera_worldToViewX(camera, self->position.x);
    //dstRect.y = Camera_worldToViewY(camera, self->position.y);
    //dstRect.w = scale * srcRect->w / self->pixPerUnit;
    //dstRect.h = scale * srcRect->h / self->pixPerUnit;

    //const float alpha = self->opacity * BasicAnimState_getValueFloat(&(self->alphaState), &(self->alphaValues));
    //bool success = SDL_SetTextureAlphaModFloat(texture, alpha);
    //assert(success);
    //success = SDL_SetTextureBlendMode(texture, self->blendMode);
    //assert(success);

    //Utils_RenderTextureRotated(
    //    g_renderer, texture, srcRect, &dstRect, self->anchor,
    //    -(self->angle), self->flip
    //);
}

void Particle_setLifetime(Particle* self, float lifetime)
{
    self->lifetime = lifetime;
    self->remainingLifetime = lifetime;
    self->alphaState.cycleTime = lifetime;
    self->scaleState.cycleTime = lifetime;
}

void Particle_setDelay(Particle* self, float delay)
{
    SpriteAnimState_playWithDelay(&(self->animState), delay);
    BasicAnimState_playWithDelay(&(self->alphaState), delay);
    BasicAnimState_playWithDelay(&(self->scaleState), delay);
}

ParticleSystem* ParticleSystem_create(SpriteAnimManager* animManager, int capacity)
{
    ParticleSystem* self = (ParticleSystem*)calloc(1, sizeof(ParticleSystem));
    ASSERT_NEW(self);

    self->m_animManager = animManager;
    self->m_particleCount = 0;
    self->m_poolCapacity = capacity;
    self->m_particlePool = (Particle*)calloc(capacity, sizeof(Particle));
    ASSERT_NEW(self->m_particlePool);

    return self;
}

void ParticleSystem_destroy(ParticleSystem* self)
{
    if (!self) return;
    free(self->m_particlePool);
    free(self);
}

void ParticleSystem_emitParticle(ParticleSystem* self, const Particle* particle)
{
    assert(self && "self must not be NULL");
    if (self->m_particleCount >= self->m_poolCapacity) return;

    self->m_particlePool[self->m_particleCount] = *particle;
    self->m_particleCount++;
}

void ParticleSystem_update(ParticleSystem* self, float dt)
{
    SpriteAnimManager* animManager = self->m_animManager;
    for (int i = 0; i < self->m_particleCount;)
    {
        Particle* particle = &(self->m_particlePool[i]);
        const bool isAlive = Particle_update(particle, animManager, dt);
        if (isAlive == false)
        {
            // Swap with the last alive particle
            self->m_particlePool[i] = self->m_particlePool[self->m_particleCount - 1];
            self->m_particleCount--;
        }
        else i++;
    }
}

void ParticleSystem_render(ParticleSystem* self, GraphicsSystem* graphicsSystem)
{
    SpriteAnimManager* animManager = self->m_animManager;
    for (int i = 0; i < self->m_particleCount; i++)
    {
        Particle* particle = &(self->m_particlePool[i]);
        Particle_render(particle, graphicsSystem, animManager);
    }
}
