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
    //
    SPRITE_COUNT,
} SpriteID;

typedef enum GameFontId
{
    FONT_NORMAL,
    FONT_BIG,
    FONT_BOLD,
    FONT_BIG_BOLD,
    FONT_TITLE,
    //
    FONT_COUNT,
} GameFontId;

typedef enum GameAudioId
{
    AUDIO_UI_SELECT,
    AUDIO_UI_VALIDATE,
    AUDIO_UI_CANCEL,
    AUDIO_UI_PAUSE,
    //
    AUDIO_COUNT,
} GameAudioId;

typedef enum GameAnimId
{
    //
    ANIM_COUNT,
} GameAnimId;

void Game_addAssets(AssetManager* assets, SpriteAnimManager* animManager);
