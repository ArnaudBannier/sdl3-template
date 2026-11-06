/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/scene_manager.h"
#include "game/core/game_context.h"
#include "game/ui/ui_manager.h"

#include "game/puzzle/puzzle_graphics.h"

typedef struct MainScene
{
    Scene base;
    UIManager* uiManager;
    PuzzleGraphics* puzzleGraphics;
} MainScene;

MainScene* MainScene_create(GameEngine* engine);
