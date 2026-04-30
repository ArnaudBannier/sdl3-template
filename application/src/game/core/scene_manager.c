/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/scene_manager.h"
#include "game/core/game_engine.h"

void Scene_init(void* selfPtr, GameEngine* engine)
{
    Scene* self = (Scene*)selfPtr;
    memset(self, 0, sizeof(Scene));
    self->m_magicNumber = MAGIC_NUMBER_SCENE;
    self->m_engine = engine;
}

bool Scene_isValid(void* selfPtr)
{
    if (!selfPtr) return false;
    Scene* scene = (Scene*)selfPtr;
    if (scene->m_magicNumber != MAGIC_NUMBER_SCENE) return false;
    if (scene->m_engine == NULL) return false;
    if (scene->m_onStart == NULL) return false;
    if (scene->m_onQuit == NULL) return false;
    if (scene->m_onUpdate == NULL) return false;
    if (scene->m_onFixedUpdate == NULL) return false;
    return true;
}

GameEngine* Scene_getEngine(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    Scene* scene = (Scene*)selfPtr;
    return scene->m_engine;
}

GameContext* Scene_getContext(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    Scene* scene = (Scene*)selfPtr;
    return GameEngine_getContext(scene->m_engine);
}

SceneManager* SceneManager_create(GameEngine* engine)
{
    SceneManager* self = (SceneManager*)calloc(1, sizeof(SceneManager));
    ASSERT_NEW(self);

    self->m_engine = engine;
    self->m_currSceneId = -1;
    self->m_nextSceneId = -1;
    return self;
}

void SceneManager_destroy(SceneManager* self)
{
    if (!self) return;

    for (int i = 0; i < MAX_SCENE_COUNT; i++)
    {
        Scene* scene = self->m_scenes[i];
        if (scene && scene->m_onDestroy)
        {
            scene->m_onDestroy(scene);
        }
        free(scene);
    }
    free(self);
}

static void SceneManager_applySceneChange(SceneManager* self)
{
    assert(self && "self must not be NULL");
    assert(self->m_currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->m_nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    if (self->m_nextSceneId != self->m_currSceneId)
    {
        Scene* currScene = NULL;  
        Scene* nextScene = NULL;
        if (self->m_currSceneId >= 0)
        {
            currScene = self->m_scenes[self->m_currSceneId];
        }
        if (self->m_nextSceneId >= 0)
        {
            nextScene = self->m_scenes[self->m_nextSceneId];
        }

        if (currScene && currScene->m_onQuit)
        {
            currScene->m_onQuit(currScene);
        }
        if (nextScene && nextScene->m_onStart)
        {
            nextScene->m_onStart(nextScene);
        }
        self->m_currSceneId = self->m_nextSceneId;
    }
}

void SceneManager_update(SceneManager* self, float dt)
{
    assert(self && "self must not be NULL");
    assert(self->m_currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->m_nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    SceneManager_applySceneChange(self);

    if (self->m_currSceneId >= 0)
    {
        Scene* currScene = self->m_scenes[self->m_currSceneId];
        if (currScene && currScene->m_onUpdate)
        {
            currScene->m_onUpdate(currScene, dt);
        }
    }
}

void SceneManager_fixedUpdate(SceneManager* self, float timeStep)
{
    assert(self && "self must not be NULL");
    assert(self->m_currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->m_nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    SceneManager_applySceneChange(self);

    if (self->m_currSceneId >= 0)
    {
        Scene* currScene = self->m_scenes[self->m_currSceneId];
        if (currScene && currScene->m_onUpdate)
        {
            currScene->m_onFixedUpdate(currScene, timeStep);
        }
    }
}

void SceneManager_addScene(SceneManager* self, void* scenePtr, int sceneId)
{
    assert(self && "self must not be NULL");
    assert(sceneId >= 0 && sceneId < MAX_SCENE_COUNT && "sceneId must be within valid range");
    assert(Scene_isValid(scenePtr) && "scene must be valid");

    Scene* prevScene = self->m_scenes[sceneId];
    if (prevScene)
    {

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene with ID %d already exists. It will be overwritten.\n", sceneId);
        if (prevScene && prevScene->m_onDestroy)
        {
            prevScene->m_onDestroy(prevScene);
        }
        free(prevScene);
    }
    
    Scene* scene = (Scene*)scenePtr;
    self->m_scenes[sceneId] = scene;
}

void SceneManager_changeScene(SceneManager* self, int sceneId)
{
    assert(self && "self must not be NULL");
    assert(sceneId >= 0 && sceneId < MAX_SCENE_COUNT && "sceneId must be within valid range");
    self->m_nextSceneId = sceneId;
}

