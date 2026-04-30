/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/title_ui_manager.h"

typedef struct GameContext GameContext;

typedef struct TitleMainMenu
{
    GameContext* m_context;
    TitleUIManager* m_uiManager;
    UIFocusManager* m_focusManager;
    UIObject* m_mainPanel;
} TitleMainMenu;

TitleMainMenu* TitleMainMenu_create(GameContext* context, TitleUIManager* manager);
void TitleMainMenu_destroy(TitleMainMenu* self);
void TitleMainMenu_update(TitleMainMenu* self);
