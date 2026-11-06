/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/game_engine.h"
#include "game/core/game_context.h"
#include "game/game_config.h"

static void GameEngine_update(GameEngine* self);
static void GameEngine_updateInput(GameEngine* self);
static void GameEngine_updateGame(GameEngine* self, float dt);
static void GameEngine_fixedUpdateGame(GameEngine* self, float timeStep);

static void GameEngine_render(GameEngine* self);

GameEngine* g_gameEngine = NULL;

GameEngine* GameEngine_create()
{
    GameEngine* self = (GameEngine*)calloc(1, sizeof(GameEngine));
    ASSERT_NEW(self);

    self->context = GameContext_create();
    self->sceneManager = SceneManager_create(self);
    self->realtime = true;
    self->timeStepMS = 10; // 100 FPS
    self->aspectRatio = 16.f / 9.f;

    return self;
}

void GameEngine_destroy(GameEngine* self)
{
    if (!self) return;
    SceneManager_destroy(self->sceneManager);
    GameContext_destroy(self->context);
    free(self);
}

void GameEngine_mainLoop(GameEngine* self)
{
    assert(self && "self must not be NULL");
    Timer_setTimeScale(g_engine.time, 1.f);

    while (true)
    {
        // Met à jour le temps
        Timer_update(g_engine.time);

        Input* input = GameContext_getInput(self->context);
        if (input->app.quitPressed) return;

        GameEngine_update(self);

        if (self->quit) return;

        // Rend la scène
        GameEngine_render(self);

        // Affiche le nouveau rendu
        SDL_RenderPresent(g_engine.sdl.renderer);
    }
}

void GameEngine_quit(GameEngine* self)
{
    self->quit = true;
}

static void GameEngine_update(GameEngine* self)
{
    assert(self && "self must not be NULL");
    SDL_Renderer* renderer = g_engine.sdl.renderer;
    GameContext* context = self->context;
    Viewport* viewport = GameContext_getViewport(context);
    AssetManager* assetManager = GameContext_getAssetManager(context);
    ParticleSystem* particleSystem = GameContext_getParticleSystem(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);

    GameEngine_updateInput(self);

    Game_updateSizes();
    Viewport_updateMain(viewport, renderer, self->aspectRatio);
    Viewport_apply(viewport, renderer);

    const float renderScale = g_engine.sizes.mainRenderScale;
    AssetManager_updateFontSizes(assetManager, renderScale);
    GraphicsSystem_setRenderScale(graphicsSystem, renderScale);

    const Uint64 deltaMS = Timer_getDeltaMS(g_engine.time);
    const Uint64 timeStepMS = self->timeStepMS;

    UISystem_update(uiSystem);
    if (self->realtime)
    {
        // Fixed update
        self->stepAccuMS += deltaMS;
        while (self->stepAccuMS >= timeStepMS)
        {
            self->stepAccuMS -= timeStepMS;
            GameEngine_fixedUpdateGame(self, (float)timeStepMS / 1000.f);
        }
        context->stepAlpha = (float)self->stepAccuMS / (float)timeStepMS;

        // Update
        GameEngine_updateGame(self, (float)deltaMS / 1000.f);
        ParticleSystem_update(particleSystem, (float)deltaMS / 1000.f);
    }
    else
    {
        context->stepAlpha = 1.f;
        if (self->makeStep)
        {
            self->makeStep = false;
            GameEngine_fixedUpdateGame(self, (float)timeStepMS / 1000.f);
            GameEngine_updateGame(self, (float)timeStepMS / 1000.f);
            ParticleSystem_update(particleSystem, (float)timeStepMS / 1000.f);
        }
    }
    UISystem_update(uiSystem);
}

static void GameEngine_updateInput(GameEngine* self)
{
    assert(self && "self must not be NULL");
    Input* input = GameContext_getInput(self->context);
    Input_update(input);

    if (input->debug.makeStepPressed)
    {
        self->makeStep = true;
    }
    if (input->debug.changeModePressed)
    {
        self->realtime = !self->realtime;
    }
    if (input->debug.objectGizmosPressed)
    {
        self->drawObjectGizmos = !self->drawObjectGizmos;
    }
    if (input->debug.uiGizmosPressed)
    {
        self->drawUIGizmos = !self->drawUIGizmos;
    }
}

static void GameEngine_updateGame(GameEngine* self, float dt)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->context;
    SceneManager* sceneManager = GameEngine_getSceneManager(self);

    SceneManager_update(sceneManager, dt);
}

static void GameEngine_fixedUpdateGame(GameEngine* self, float timeStep)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->context;
    SceneManager* sceneManager = GameEngine_getSceneManager(self);

    SceneManager_fixedUpdate(sceneManager, timeStep);
}

static void GameEngine_render(GameEngine* self)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->context;
    SDL_Renderer* renderer = g_engine.sdl.renderer;
    ParticleSystem* particleSystem = GameContext_getParticleSystem(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    GraphicsSystem* graphics = GameContext_getGraphicsSystem(context);
    GizmosSystem* gizmos = GameContext_getGizmosSystem(context);
    Camera* camera = GameContext_getCamera(context);
    SceneManager* sceneManager = GameEngine_getSceneManager(self);

    Camera_update(camera);

    // Efface le rendu précédent
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color bgColor = g_colors.gray9;
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
    SDL_RenderFillRect(renderer, NULL);

    GraphicsSystem_clear(graphics);
    UISystem_render(uiSystem, graphics);
    ParticleSystem_render(particleSystem, graphics);
    SceneManager_render(sceneManager);

    // Graphics
    GraphicsSystem_render(graphics, renderer, camera, uiSystem);

    // Gizmos
    if (self->drawObjectGizmos)
    {
        GizmosSystem_clear(gizmos);
        GizmosSystem_render(gizmos, renderer, camera, uiSystem);
    }

    if (self->drawUIGizmos)
    {
        GizmosSystem_clear(gizmos);
        UISystem_drawGizmos(uiSystem, gizmos);
        GizmosSystem_render(gizmos, renderer, camera, uiSystem);
    }
}
