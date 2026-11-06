/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/ui_manager.h"

typedef struct GameContext GameContext;
typedef struct SettingsPage
{
    GameContext* m_context;
    UIManager* m_uiManager;
    UIFocusManager* m_focusManager;

    UIObject* m_mainPanel;
    UIList* m_fullscreenList;
    UIList* m_musicGainList;
    UIList* m_sfxGainList;
    UIList* m_uiGainList;

    float m_prevMusicGain;
    float m_prevSfxGain;
    float m_prevUIGain;
} SettingsPage;

SettingsPage* SettingsPage_create(GameContext* context, UIManager* manager);
void SettingsPage_destroy(SettingsPage* self);
void SettingsPage_update(SettingsPage* self);
