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
    GameContext* context;
    SceneManager* sceneManager;

    Uint64 timeStepMS;
    Uint64 stepAccuMS;
    bool drawObjectGizmos;
    bool drawUIGizmos;
    bool makeStep;
    bool realtime;
    bool quit;
    float aspectRatio;
} GameEngine;

GameEngine* GameEngine_create();
void GameEngine_destroy(GameEngine* self);

void GameEngine_mainLoop(GameEngine* self);
void GameEngine_quit(GameEngine* self);

INLINE GameContext* GameEngine_getContext(GameEngine* self)
{
    assert(self && "self must not be NULL");
    return self->context;
}

INLINE SceneManager* GameEngine_getSceneManager(GameEngine* self)
{
    assert(self && "self must not be NULL");
    return self->sceneManager;
}

extern GameEngine* g_gameEngine;
