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
#include "game/ui/title_ui_manager.h"

typedef struct TitleScene
{
    Scene m_base;
    TitleUIManager* m_uiManager;
} TitleScene;

TitleScene* TitleScene_create(GameEngine* engine);
