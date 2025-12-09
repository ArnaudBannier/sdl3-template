/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef enum SpriteID
{
    SPRITE_UI_BASE,
    SPRITE_GAME,
    //
    SPRITE_COUNT,
} SpriteID;

typedef enum FontID
{
    FONT_NORMAL,
    FONT_BIG,
    FONT_BOLD,
    FONT_BIG_BOLD,
    //
    FONT_COUNT,
} FontID;

void Game_addAssets(AssetManager* assets);
