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
#include "game/core/game_context.h"
#include "game/core/scene_manager.h"

typedef struct GameEngine
{
    GameContext* m_context;
    SceneManager* m_sceneManager;

    Uint64 m_timeStepMS;
    Uint64 m_stepAccuMS;
    bool m_drawObjectGizmos;
    bool m_drawUIGizmos;
    bool m_makeStep;
    bool m_realtime;
    bool m_quit;
    float m_aspectRatio;
} GameEngine;

GameEngine* GameEngine_create();
void GameEngine_destroy(GameEngine* self);

void GameEngine_mainLoop(GameEngine* self);
void GameEngine_quit(GameEngine* self);

INLINE GameContext* GameEngine_getContext(GameEngine* self)
{
    assert(self && "self must not be NULL");
    return self->m_context;
}

INLINE SceneManager* GameEngine_getSceneManager(GameEngine* self)
{
    assert(self && "self must not be NULL");
    return self->m_sceneManager;
}

extern GameEngine* g_gameEngine;
