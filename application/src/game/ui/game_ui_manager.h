/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct Scene Scene;
typedef struct GameSettingsPage GameSettingsPage;
typedef struct GameTitlePage GameTitlePage;
typedef struct GameHUD GameHUD;

typedef enum GameUIAction
{
    GAME_UI_ACTION_NONE = 0,
    GAME_UI_ACTION_START,
    GAME_UI_ACTION_OPEN_SETTINGS,
    GAME_UI_ACTION_OPEN_MAIN,
    GAME_UI_ACTION_CHANGE_FULLSCREEN,
    GAME_UI_ACTION_APPLY_SETTINGS,
    GAME_UI_ACTION_QUIT,
    GAME_UI_ACTION_RESTART_LEVEL,
    GAME_UI_ACTION_GENERATE_LEVEL,
    GAME_UI_ACTION_SHOW_SOLUTION,
    GAME_UI_ACTION_HIDE_SOLUTION,
    GAME_UI_ACTION_SHOW_GENERATION_SETTINGS,
    GAME_UI_ACTION_HIDE_GENERATION_SETTINGS,
    GAME_UI_ACTION_PREV_STEP,
    GAME_UI_ACTION_NEXT_STEP,
    GAME_UI_ACTION_COUNT
} GameUIAction;

typedef struct GameUIManager
{
    Scene* m_scene;
    UICanvas* m_canvas;
    GameTitlePage* m_mainPage;
    GameSettingsPage* m_settingsPage;
    GameUIAction m_nextAction;
} GameUIManager;

GameUIManager* GameUIManager_create(Scene* scene);
void GameUIManager_destroy(GameUIManager* self);

void GameUIManager_render(GameUIManager* self);
void GameUIManager_update(GameUIManager* self, UIInput* input);

