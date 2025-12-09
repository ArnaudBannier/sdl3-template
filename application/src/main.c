/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "settings.h"

#include "common/common.h"
#include "game/game_config.h"
#include "game/input.h"
#include "game/scene.h"

#include <SDL3/SDL_main.h>


#ifdef WINDOW_FHD
#define WINDOW_WIDTH   FHD_WIDTH
#define WINDOW_HEIGHT  FHD_HEIGHT
#else
#define WINDOW_WIDTH   HD_WIDTH
#define WINDOW_HEIGHT  HD_HEIGHT
#endif
#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT

int main(int argc, char* argv[])
{
    //--------------------------------------------------------------------------
    // Initialisation

    srand((unsigned int)time(NULL));

    // Initialisation de la SDL
    const GameInitParams initParams = {
        .sdlFlags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD,
        .projectRootPath = PROJECT_ROOT_PATH,
        .assetsPath = ASSETS_PATH,
    };
    Game_init(&initParams);
    GameConfig_init();

    SDL_SetLogPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_INFO);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_INFO);

    // Crée la fenêtre et le moteur de rendu
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
    Game_createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, u8"SDL3 Template", windowFlags);
    Game_createRenderer(LOGICAL_WIDTH, LOGICAL_HEIGHT);

    //--------------------------------------------------------------------------
    // Creation des assets

#ifndef DEPLOY
    Game_createAssetsFromDev();
#endif

    //--------------------------------------------------------------------------
    // Boucle de jeu

    Scene* scene = NULL;
    bool quitGame = false;
    while (quitGame == false)
    {
        switch (g_gameConfig.nextScene)
        {
        case GAME_SCENE_MAIN:
            scene = Scene_create();
            Scene_mainLoop(scene);
            Scene_destroy(scene);
            scene = NULL;

        case GAME_SCENE_QUIT:
        default:
            quitGame = true;
            break;
        }
    }

    //--------------------------------------------------------------------------
    // Libération de la mémoire

    Scene_destroy(scene); scene = NULL;
    Game_destroyRenderer();
    Game_destroyWindow();
    Game_quit();

    return EXIT_SUCCESS;
}
