/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/timer.h"
#include "utils/utils.h"
#include "utils/colors.h"

#define DEFAULT_UI_LAYER 1000

#define MIX_CHANNEL_COUNT 16
typedef struct AssetManager AssetManager;
typedef struct GamePaths
{
    char* base;
    char* assets;
    char* projectRoot;
} GamePaths;

typedef struct GameSizes
{
    Vec2 uiSize;
    Vec2 uiPixelsPerUnit;
    Vec2 viewportSize;
    float mainRenderScale;
    float defaultRenderWidth;
} GameSizes;

typedef struct SDLCommon
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    MIX_Mixer* mixer;
} SDLCommon;

typedef struct EngineCommon
{
    SDLCommon sdl;
    GameSizes sizes;
    GamePaths paths;
    Timer* time;
} EngineCommon;

extern EngineCommon g_engine;

typedef struct GameInitParams
{
    SDL_InitFlags sdlFlags;
    //int mixFlags;
    const char* assetsPath;
    const char* projectRootPath;
} GameInitParams;

/// @brief Initialise les librairies utilisées par le jeu.
/// @param params les paramètres d'initialisation.
/// @return Le résultat de l'initialisation.
SDL_AppResult Game_init(const GameInitParams* params);

/// @brief Crée la fenêtre du jeu.
/// @param width largeur de la fenêtre.
/// @param height hauteur de la fenêtre.
/// @param title la titre de la fenêtre.
/// @param flags les flags SDL.
SDL_AppResult Game_createWindow(int width, int height, const char* title, Uint32 flags);

SDL_AppResult Game_createMixer();

/// @brief Crée le moteur de rendu.
SDL_AppResult Game_createRenderer();

/// @brief Détruit le moteur de rendu du jeu.
void Game_destroyRenderer();

/// @brief Détruit la fenêtre du jeu.
void Game_destroyWindow();

void Game_destroyMixer();

/// @brief Quitte les librairies utilisées par le jeu.
void Game_quit();

/// @brief Définit la couleur du moteur de rendu utilisée par les opérations draw ou fill.
/// @param color la couleur (le paramètre alpha est ignoré).
/// @param alpha l'opacité (0 pour transparant, 255 pour opaque).
void Game_setRenderDrawColor(SDL_Color color, Uint8 alpha);

void Game_createAssetsFromDev();

void Game_updateSizes();

void Memcpy(void* const dst, size_t dstSize, const void* src, size_t srcSize);
char* Strdup(const char* src);

void Game_obfuscateMem(void* memory, Uint64 size);
void Game_retriveMem(void* memory, Uint64 size);
