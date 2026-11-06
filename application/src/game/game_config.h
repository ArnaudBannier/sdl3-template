/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"

typedef enum PuzzleState
{
    PUZZLE_STATE_PLAYING,
    PUZZLE_STATE_COMPLETED,
    PUZZLE_STATE_FAILED
} PuzzleState;

typedef struct PuzzleInfo
{
    PuzzleState state;
    bool enabled;
} PuzzleInfo;

typedef struct GameConfig
{
    float musicGain;
    float sfxGain;
    float uiGain;

    PuzzleInfo puzzle;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
