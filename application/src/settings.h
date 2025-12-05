/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine.h"

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
