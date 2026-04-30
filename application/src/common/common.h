/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

#define MAX_PLAYER_COUNT 2
#define PIX_PER_UNIT 48.0f
#define PIX_TO_WORLD (1.0f / 48.0f)

//#define TEST_LEVEL
//#define SKIP_MAIN_MENU

#define DAMAGE_SMALL 5.f
#define DAMAGE_MEDIUM 10.f
#define DAMAGE_BIG 20.f
#define DAMAGE_HUGE 40.f

#ifdef TEST_LEVEL
#  define DAMAGE_PLAYER_DEFAULT 1000.f
#  define PLAYER_MAX_HP 10000.f
#  define PLAYER_BULLET_TIME 1.f / 50.f
#else
#  define DAMAGE_PLAYER_DEFAULT DAMAGE_SMALL
#  define PLAYER_MAX_HP 100.f
#  define PLAYER_BULLET_TIME 1.f / 5.f
#endif

typedef enum GameObjectType
{
    GAME_TYPE_UNDEFINED,
    GAME_TYPE_PLAYER,
    GAME_TYPE_LINEAR_BULLET,
    GAME_TYPE_HOMING_MISSILE,
    GAME_TYPE_FIGHTER,
    GAME_TYPE_FRIGATE,
    GAME_TYPE_ITEM,
} GameObjectType;

typedef enum GameObjectCategory
{
    GAME_CATEGORY_PLAYER = 1 << 0,
    GAME_CATEGORY_PLAYER_BULLET = 1 << 1,
    GAME_CATEGORY_ENEMY = 1 << 2,
    GAME_CATEGORY_ENEMY_BULLET = 1 << 3,
    GAME_CATEGORY_ITEM = 1 << 4,
} GameObjectCategory;

typedef enum GameLayer
{
    GAME_LAYER_BACKGROUND,
    GAME_LAYER_PLAYER,
    GAME_LAYER_ITEM,
    GAME_LAYER_BULLET,
    GAME_LAYER_ENEMY,
    GAME_LAYER_PARTICLE,
    GAME_LAYER_FOREGROUND,
    GAME_LAYER_UI = DEFAULT_UI_LAYER
} GameLayer;

typedef enum GameAudioGroup
{
    GAME_AUDIO_GROUP_MUSIC,
    GAME_AUDIO_GROUP_SFX,
    GAME_AUDIO_GROUP_UI,
    //
    GAME_AUDIO_GROUP_COUNT,
} GameAudioGroup;

typedef enum SceneType
{
    SCENE_TYPE_TITLE,
    SCENE_TYPE_LEVEL,
} SceneType;

#define GAME_MUSIC_TRACK_COUNT 2
#define GAME_SFX_TRACK_COUNT 12
#define GAME_UI_TRACK_COUNT 4

typedef enum LevelID
{
    LEVEL_1,
    LEVEL_2,
    //
    LEVEL_COUNT,
} LevelID;
