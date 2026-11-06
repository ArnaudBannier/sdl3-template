/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/main_scene.h"
#include "game/core/game_engine.h"

static void MainScene_onStart(void* selfPtr);
static void MainScene_onQuit(void* selfPtr);
static void MainScene_onUpdate(void* selfPtr, float dt);
static void MainScene_onFixedUpdate(void* selfPtr, float timeStep);
static void MainScene_onRender(void* selfPtr);
static void MainScene_onDestroy(void* selfPtr);

MainScene* MainScene_create(GameEngine* engine)
{
    MainScene* self = (MainScene*)calloc(1, sizeof(MainScene));
    ASSERT_NEW(self);

    Scene* selfScene = (Scene*)self;
    Scene_init(selfScene, engine);
    selfScene->m_onStart = MainScene_onStart;
    selfScene->m_onQuit = MainScene_onQuit;
    selfScene->m_onUpdate = MainScene_onUpdate;
    selfScene->m_onFixedUpdate = MainScene_onFixedUpdate;
    selfScene->m_onRender = MainScene_onRender;
    selfScene->m_onDestroy = MainScene_onDestroy;

    GameContext* context = GameEngine_getContext(engine);
    self->uiManager = UIManager_create(context);

    self->puzzleGraphics = PuzzleGraphics_create(context);

    return self;
}

static void MainScene_onStart(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    UIManager_openPage(self->uiManager, TITLE_UI_PAGE_MAIN_MENU);

    AssetManager* assets = GameContext_getAssetManager(context);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);
    AudioSystem_stopGroup(audioSystem, GAME_AUDIO_GROUP_MUSIC, 1000);

    Timer_setTimeScale(g_engine.time, 1.f);
}

static void MainScene_onQuit(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    UIManager_closePages(self->uiManager);
    UIManager_update(self->uiManager);
}

static void MainScene_onUpdate(void* selfPtr, float dt)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;

    UIManager_update(self->uiManager);

    PuzzleGraphics_update(self->puzzleGraphics);
}

static void MainScene_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;
}

static void MainScene_onRender(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;

    PuzzleGraphics_render(self->puzzleGraphics);
}

static void MainScene_onDestroy(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    MainScene* self = (MainScene*)selfPtr;
    UIManager_destroy(self->uiManager);
    PuzzleGraphics_destroy(self->puzzleGraphics);
}
