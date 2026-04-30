/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/title_scene.h"
#include "game/core/game_engine.h"

static void TitleScene_onStart(void* selfPtr);
static void TitleScene_onQuit(void* selfPtr);
static void TitleScene_onUpdate(void* selfPtr, float dt);
static void TitleScene_onFixedUpdate(void* selfPtr, float timeStep);
static void TitleScene_onDestroy(void* selfPtr);

TitleScene* TitleScene_create(GameEngine* engine)
{
    TitleScene* self = (TitleScene*)calloc(1, sizeof(TitleScene));
    ASSERT_NEW(self);

    Scene* selfScene = (Scene*)self;
    Scene_init(selfScene, engine);
    selfScene->m_onStart = TitleScene_onStart;
    selfScene->m_onQuit = TitleScene_onQuit;
    selfScene->m_onUpdate = TitleScene_onUpdate;
    selfScene->m_onFixedUpdate = TitleScene_onFixedUpdate;
    selfScene->m_onDestroy = TitleScene_onDestroy;

    GameContext* context = GameEngine_getContext(engine);
    self->m_uiManager = TitleUIManager_create(context);

    return self;
}

static void TitleScene_onStart(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    TitleUIManager_openPage(self->m_uiManager, TITLE_UI_PAGE_MAIN_MENU);

    AssetManager* assets = GameContext_getAssetManager(context);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);
    AudioSystem_stopGroup(audioSystem, GAME_AUDIO_GROUP_MUSIC, 1000);

    Timer_setTimeScale(g_engine.time, 1.f);
}

static void TitleScene_onQuit(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    TitleUIManager_closePages(self->m_uiManager);
    TitleUIManager_update(self->m_uiManager);
}

static void TitleScene_onUpdate(void* selfPtr, float dt)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;

    TitleUIManager_update(self->m_uiManager);
}

static void TitleScene_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
}

static void TitleScene_onDestroy(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    TitleUIManager_destroy(self->m_uiManager);
}
