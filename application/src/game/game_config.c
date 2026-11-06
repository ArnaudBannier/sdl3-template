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

    g_gameConfig.puzzle.state = PUZZLE_STATE_PLAYING;
    g_gameConfig.puzzle.enabled = false;
}
