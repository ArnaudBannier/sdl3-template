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

    self->m_context = GameContext_create();
    self->m_sceneManager = SceneManager_create(self);
    self->m_realtime = true;
    self->m_timeStepMS = 10; // 100 FPS
    self->m_aspectRatio = 16.f / 9.f;

    return self;
}

void GameEngine_destroy(GameEngine* self)
{
    if (!self) return;
    SceneManager_destroy(self->m_sceneManager);
    GameContext_destroy(self->m_context);
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

        Input* input = GameContext_getInput(self->m_context);
        if (input->app.quitPressed) return;

        GameEngine_update(self);

        if (self->m_quit) return;

        // Rend la scène
        GameEngine_render(self);

        // Affiche le nouveau rendu
        SDL_RenderPresent(g_engine.sdl.renderer);
    }
}

void GameEngine_quit(GameEngine* self)
{
    self->m_quit = true;
}

static void GameEngine_update(GameEngine* self)
{
    assert(self && "self must not be NULL");
    SDL_Renderer* renderer = g_engine.sdl.renderer;
    GameContext* context = self->m_context;
    Viewport* viewport = GameContext_getViewport(context);
    AssetManager* assetManager = GameContext_getAssetManager(context);
    ParticleSystem* particleSystem = GameContext_getParticleSystem(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);

    GameEngine_updateInput(self);

    Game_updateSizes();
    Viewport_updateMain(viewport, renderer, self->m_aspectRatio);
    Viewport_apply(viewport, renderer);

    const float renderScale = g_engine.sizes.mainRenderScale;
    AssetManager_updateFontSizes(assetManager, renderScale);
    GraphicsSystem_setRenderScale(graphicsSystem, renderScale);

    const Uint64 deltaMS = Timer_getDeltaMS(g_engine.time);
    const Uint64 timeStepMS = self->m_timeStepMS;

    UISystem_update(uiSystem);
    if (self->m_realtime)
    {
        // Fixed update
        self->m_stepAccuMS += deltaMS;
        while (self->m_stepAccuMS >= timeStepMS)
        {
            self->m_stepAccuMS -= timeStepMS;
            GameEngine_fixedUpdateGame(self, (float)timeStepMS / 1000.f);
        }
        context->m_stepAlpha = (float)self->m_stepAccuMS / (float)timeStepMS;

        // Update
        GameEngine_updateGame(self, (float)deltaMS / 1000.f);
        ParticleSystem_update(particleSystem, (float)deltaMS / 1000.f);
    }
    else
    {
        context->m_stepAlpha = 1.f;
        if (self->m_makeStep)
        {
            self->m_makeStep = false;
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
    Input* input = GameContext_getInput(self->m_context);
    Input_update(input);

    if (input->debug.makeStepPressed)
    {
        self->m_makeStep = true;
    }
    if (input->debug.changeModePressed)
    {
        self->m_realtime = !self->m_realtime;
    }
    if (input->debug.objectGizmosPressed)
    {
        self->m_drawObjectGizmos = !self->m_drawObjectGizmos;
    }
    if (input->debug.uiGizmosPressed)
    {
        self->m_drawUIGizmos = !self->m_drawUIGizmos;
    }
}

static void GameEngine_updateGame(GameEngine* self, float dt)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->m_context;
    SceneManager* sceneManager = GameEngine_getSceneManager(self);

    SceneManager_update(sceneManager, dt);
}

static void GameEngine_fixedUpdateGame(GameEngine* self, float timeStep)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->m_context;
    SceneManager* sceneManager = GameEngine_getSceneManager(self);

    SceneManager_fixedUpdate(sceneManager, timeStep);
}

static void GameEngine_render(GameEngine* self)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->m_context;
    SDL_Renderer* renderer = g_engine.sdl.renderer;
    ParticleSystem* particleSystem = GameContext_getParticleSystem(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    GraphicsSystem* graphics = GameContext_getGraphicsSystem(context);
    GizmosSystem* gizmos = GameContext_getGizmosSystem(context);
    Camera* camera = GameContext_getCamera(context);

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

    // Graphics
    GraphicsSystem_render(graphics, renderer, camera, uiSystem);

    // Gizmos
    if (self->m_drawObjectGizmos)
    {
        GizmosSystem_clear(gizmos);
        GizmosSystem_render(gizmos, renderer, camera, uiSystem);
    }

    if (self->m_drawUIGizmos)
    {
        GizmosSystem_clear(gizmos);
        UISystem_drawGizmos(uiSystem, gizmos);
        GizmosSystem_render(gizmos, renderer, camera, uiSystem);
    }
}
