/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/anim/basic_anim_state.h"
#include "core/anim/sprite_anim_state.h"
#include "core/anim/sprite_anim_manager.h"
#include "core/graphics_system.h"
#include "core/components.h"
#include "core/camera.h"
#include "utils/utils.h"

typedef struct Particle
{
    Transform transform;
    RenderSortingLayer sortingLayer;
    RenderBlendMod blendMod;
    RenderAnchor anchor;
    RenderFlip flip;
    RenderDim dimensions;
    SpriteAnimState animState;

    float lifetime;
    float remainingLifetime;
    float angularVelocity;

    Vec2 velocity;
    Vec2 gravity;
    Vec2 damping;

    BasicAnimState alphaState;
    BasicAnimState scaleState;
    LerpValuesFloat alphaValues;
    LerpValuesFloat scaleValues;
} Particle;

void Particle_init(Particle* self, SpriteAnimManager* animManager, AnimId id);

void Particle_setLifetime(Particle* self, float lifetime);
void Particle_setDelay(Particle* self, float delay);

typedef struct ParticleSystem
{
    SpriteAnimManager* m_animManager;
    Particle* m_particlePool;
    int m_particleCount;
    int m_poolCapacity;
} ParticleSystem;

ParticleSystem* ParticleSystem_create(SpriteAnimManager* animManager, int capacity);
void ParticleSystem_destroy(ParticleSystem* self);
void ParticleSystem_emitParticle(ParticleSystem* self, const Particle* particle);
void ParticleSystem_update(ParticleSystem* self, float dt);
void ParticleSystem_render(ParticleSystem* self, GraphicsSystem* graphicsSystem);
