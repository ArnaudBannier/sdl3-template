/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "settings.h"

#include "common/common.h"
#include "game/game_config.h"
#include "game/input.h"
#include "game/core/game_engine.h"
#include "game/core/game_context.h"
#include "game/scene/title_scene.h"

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
    Game_createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Space Pixels", windowFlags);
    Game_createRenderer();
    Game_createMixer();

    //--------------------------------------------------------------------------
    // Creation des assets

#ifndef DEPLOY
    Game_createAssetsFromDev();
#endif

    //--------------------------------------------------------------------------
    // Création du moteur de jeu et des scènes

    g_gameEngine = GameEngine_create();
    SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);

    TitleScene* titleScene = TitleScene_create(g_gameEngine);
    SceneManager_addScene(sceneManager, titleScene, SCENE_TYPE_TITLE);

    GameContext* context = GameEngine_getContext(g_gameEngine);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, g_gameConfig.musicGain);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_SFX, g_gameConfig.sfxGain);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_UI, g_gameConfig.uiGain);

    //--------------------------------------------------------------------------
    // Boucle de jeu
    
    SceneManager_changeScene(sceneManager, SCENE_TYPE_TITLE);
    GameEngine_mainLoop(g_gameEngine);

    //--------------------------------------------------------------------------
    // Libération de la mémoire

    GameEngine_destroy(g_gameEngine);
    g_gameEngine = NULL;

    Game_destroyMixer();
    Game_destroyRenderer();
    Game_destroyWindow();
    Game_quit();

    return EXIT_SUCCESS;
}
