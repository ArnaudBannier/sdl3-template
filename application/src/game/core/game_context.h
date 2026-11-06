/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/assets.h"
#include "common/common.h"
#include "game/input.h"
#include "game/game_config.h"

typedef struct GameContext
{
    AssetManager* assets;
    SpriteAnimManager* animManager;
    AudioSystem* audioSystem;
    Viewport* viewport;
    Camera* camera;
    Input* input;
    UISystem* uiSystem;
    ParticleSystem* particleSystem;
    GraphicsSystem* graphicsSystem;
    GizmosSystem* gizmosSystem;

    float stepAlpha;
} GameContext;

GameContext* GameContext_create();
void GameContext_destroy(GameContext* self);

void GameContext_playSFX(GameContext* self, int audioId);
void GameContext_playSpatialSFX(GameContext* self, int audioId, Transform* transform);

INLINE AssetManager* GameContext_getAssetManager(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->assets;
}

INLINE Input* GameContext_getInput(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->input;
}

INLINE AudioSystem* GameContext_getAudioSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->audioSystem;
}

INLINE Viewport* GameContext_getViewport(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->viewport;
}

INLINE Camera* GameContext_getCamera(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->camera;
}

INLINE SpriteAnimManager* GameContext_getAnimManager(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->animManager;
}

INLINE GraphicsSystem* GameContext_getGraphicsSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->graphicsSystem;
}

INLINE GizmosSystem* GameContext_getGizmosSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->gizmosSystem;
}

INLINE ParticleSystem* GameContext_getParticleSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->particleSystem;
}

INLINE UISystem* GameContext_getUISystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->uiSystem;
}

INLINE float GameContext_getStepAlpha(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->stepAlpha;
}
