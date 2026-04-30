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
    AssetManager* m_assets;
    SpriteAnimManager* m_animManager;
    AudioSystem* m_audioSystem;
    Viewport* m_viewport;
    Camera* m_camera;
    Input* m_input;
    UISystem* m_uiSystem;
    ParticleSystem* m_particleSystem;
    GraphicsSystem* m_graphicsSystem;
    GizmosSystem* m_gizmosSystem;

    float m_stepAlpha;
} GameContext;

GameContext* GameContext_create();
void GameContext_destroy(GameContext* self);

void GameContext_addGameObject(GameContext* self, void* gameObjectPtr);
void GameContext_playSFX(GameContext* self, int audioId);
void GameContext_playSpatialSFX(GameContext* self, int audioId, Transform* transform);

INLINE AssetManager* GameContext_getAssetManager(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_assets;
}

INLINE Input* GameContext_getInput(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_input;
}

INLINE AudioSystem* GameContext_getAudioSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_audioSystem;
}

INLINE Viewport* GameContext_getViewport(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_viewport;
}

INLINE Camera* GameContext_getCamera(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_camera;
}

INLINE SpriteAnimManager* GameContext_getAnimManager(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_animManager;
}

INLINE GraphicsSystem* GameContext_getGraphicsSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_graphicsSystem;
}

INLINE GizmosSystem* GameContext_getGizmosSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_gizmosSystem;
}

INLINE ParticleSystem* GameContext_getParticleSystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_particleSystem;
}

INLINE UISystem* GameContext_getUISystem(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_uiSystem;
}

INLINE float GameContext_getStepAlpha(GameContext* self)
{
    assert(self && "self must not be NULL");
    return self->m_stepAlpha;
}
