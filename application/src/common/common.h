/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

#define MAX_PLAYER_COUNT 1

typedef enum GameScene
{
    GAME_SCENE_MAIN,
    GAME_SCENE_QUIT
} GameScene;

typedef enum SceneState
{
    SCENE_STATE_RUNNING,
    SCENE_STATE_FADING_IN,
    SCENE_STATE_FADING_OUT,
    SCENE_STATE_FINISHED
} SceneState;
