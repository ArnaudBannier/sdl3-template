/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/title_ui_manager.h"

typedef struct GameContext GameContext;
typedef struct TitleSettingsPage
{
    GameContext* m_context;
    TitleUIManager* m_uiManager;
    UIFocusManager* m_focusManager;

    UIObject* m_mainPanel;
    UIList* m_fullscreenList;
    UIList* m_musicGainList;
    UIList* m_sfxGainList;
    UIList* m_uiGainList;

    float m_prevMusicGain;
    float m_prevSfxGain;
    float m_prevUIGain;
} TitleSettingsPage;

TitleSettingsPage* TitleSettingsPage_create(GameContext* context, TitleUIManager* manager);
void TitleSettingsPage_destroy(TitleSettingsPage* self);
void TitleSettingsPage_update(TitleSettingsPage* self);
