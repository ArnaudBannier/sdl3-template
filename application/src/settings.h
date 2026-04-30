/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine.h"

#define HD_WIDTH 1280
#define HD_HEIGHT 720
#define FHD_WIDTH 1920
#define FHD_HEIGHT 1080

//#define WINDOW_FHD
//#define DEPLOY

#ifdef DEPLOY
#define PROJECT_ROOT_PATH "..\\..\\.."
#define ASSETS_PATH "assets"
#else
#define PROJECT_ROOT_PATH "..\\..\\.."
#define ASSETS_PATH "..\\..\\..\\assets"
#endif

#define MAX_UI_ELEMENT_COUNT 1024
#define MAX_BODY_COUNT 1 << 15
#define MAX_OBJECT_COUNT 1 << 15
#define MAX_SCENE_COUNT 16

#define MAGIC_NUMBER_GAME_OBJECT (uint64_t)0xAABBCCDDEEFF0001
#define MAGIC_NUMBER_SCENE (uint64_t)0xAABBCCDDEEFF0002
