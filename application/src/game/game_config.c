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
    g_gameConfig.musicGain = 0.4f;
    g_gameConfig.sfxGain = 0.4f;
    g_gameConfig.uiGain = 0.4f;

    g_gameConfig.playerCount = 1;

    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        g_gameConfig.playerScores[i] = 0.f;
        g_gameConfig.playerHPs[i] = PLAYER_MAX_HP;
    }

    g_gameConfig.level.activePlayerCount = 0;
    g_gameConfig.level.enemyCount = 0;
    g_gameConfig.level.levelId = LEVEL_1;
    g_gameConfig.level.state = GAME_STATE_PAUSED;
    g_gameConfig.level.waveIdx = 0;
}
