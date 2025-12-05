/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_core.h"


typedef struct GameConfig
{
    GameScene nextScene;

    bool showElapsedTime;
    bool inLevel;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
