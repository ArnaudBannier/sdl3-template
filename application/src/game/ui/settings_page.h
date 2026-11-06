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
    GameContext* context;
    UIManager* uiManager;
    UIFocusManager* focusManager;

    UIObject* mainPanel;
    UIList* fullscreenList;
    UIList* musicGainList;
    UIList* sfxGainList;
    UIList* uiGainList;

    float prevMusicGain;
    float prevSfxGain;
    float prevUIGain;
} SettingsPage;

SettingsPage* SettingsPage_create(GameContext* context, UIManager* manager);
void SettingsPage_destroy(SettingsPage* self);
void SettingsPage_update(SettingsPage* self);
