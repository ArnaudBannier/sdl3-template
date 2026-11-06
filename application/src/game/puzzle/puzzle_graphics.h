/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/core/game_context.h"

#define PUZZLE_ROW_COUNT 4
#define PUZZLE_COL_COUNT 6

typedef struct PuzzleGraphics
{
    GameContext* context;

    AABB puzzleAABB;
    AABB cells[PUZZLE_ROW_COUNT][PUZZLE_COL_COUNT];
    float spacing;

    int selectedRow;
    int selectedCol;
} PuzzleGraphics;

PuzzleGraphics* PuzzleGraphics_create(GameContext* context);
void PuzzleGraphics_destroy(PuzzleGraphics* self);
void PuzzleGraphics_update(PuzzleGraphics* self);
void PuzzleGraphics_render(PuzzleGraphics* self);
