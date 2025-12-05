/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/game_ui_manager.h"

typedef struct Scene Scene;

typedef struct GameMainPage
{
    Scene* m_scene;
    GameUIManager* m_manager;
    UIFocusManager* m_focusManager;

    GameUIAction m_nextAction;
    UIObject* m_mainPanel;
} GameMainPage;

GameMainPage* GameMainPage_create(Scene* scene, GameUIManager *manager);
void GameMainPage_destroy(GameMainPage* self);
void GameMainPage_update(GameMainPage* self, UIInput* input);

