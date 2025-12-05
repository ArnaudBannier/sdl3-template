/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/game_config.h"
#include "common/assets.h"
#include "common/common.h"

GameConfig g_gameConfig = { 0 };

void GameConfig_init()
{
    memset(&g_gameConfig, 0, sizeof(GameConfig));
    g_gameConfig.nextScene = GAME_SCENE_MAIN;
    g_gameConfig.inLevel = false;
}
