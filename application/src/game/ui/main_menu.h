/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/ui_manager.h"

typedef struct GameContext GameContext;

typedef struct MainMenu
{
    GameContext* m_context;
    UIManager* m_uiManager;
    UIFocusManager* m_focusManager;
    UIObject* m_mainPanel;
} MainMenu;

MainMenu* MainMenu_create(GameContext* context, UIManager* manager);
void MainMenu_destroy(MainMenu* self);
void MainMenu_update(MainMenu* self);
