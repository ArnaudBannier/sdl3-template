/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"

typedef enum LevelState
{
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_COMPLETED,
    GAME_STATE_FAILED
} LevelState;

typedef struct LevelInfo
{
    LevelState state;
    int levelId;
    int waveIdx;
    int enemyCount;
    int activePlayerCount;
    bool shouldPause;
    bool shouldResume;
} LevelInfo;

typedef struct GameConfig
{
    int playerCount;
    float playerScores[MAX_PLAYER_COUNT];
    float playerHPs[MAX_PLAYER_COUNT];

    float musicGain;
    float sfxGain;
    float uiGain;

    LevelInfo level;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
