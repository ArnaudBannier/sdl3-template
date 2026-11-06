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
    self->magicNumber = MAGIC_NUMBER_SCENE;
    self->engine = engine;
}

bool Scene_isValid(void* selfPtr)
{
    if (!selfPtr) return false;
    Scene* scene = (Scene*)selfPtr;
    if (scene->magicNumber != MAGIC_NUMBER_SCENE) return false;
    if (scene->engine == NULL) return false;
    if (scene->onStart == NULL) return false;
    if (scene->onQuit == NULL) return false;
    if (scene->onUpdate == NULL) return false;
    if (scene->onRender == NULL) return false;
    if (scene->onFixedUpdate == NULL) return false;
    return true;
}

GameEngine* Scene_getEngine(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    Scene* scene = (Scene*)selfPtr;
    return scene->engine;
}

GameContext* Scene_getContext(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    Scene* scene = (Scene*)selfPtr;
    return GameEngine_getContext(scene->engine);
}

SceneManager* SceneManager_create(GameEngine* engine)
{
    SceneManager* self = (SceneManager*)calloc(1, sizeof(SceneManager));
    ASSERT_NEW(self);

    self->engine = engine;
    self->currSceneId = -1;
    self->nextSceneId = -1;
    return self;
}

void SceneManager_destroy(SceneManager* self)
{
    if (!self) return;

    for (int i = 0; i < MAX_SCENE_COUNT; i++)
    {
        Scene* scene = self->scenes[i];
        if (scene && scene->onDestroy)
        {
            scene->onDestroy(scene);
        }
        free(scene);
    }
    free(self);
}

static void SceneManager_applySceneChange(SceneManager* self)
{
    assert(self && "self must not be NULL");
    assert(self->currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    if (self->nextSceneId != self->currSceneId)
    {
        Scene* currScene = NULL;  
        Scene* nextScene = NULL;
        if (self->currSceneId >= 0)
        {
            currScene = self->scenes[self->currSceneId];
        }
        if (self->nextSceneId >= 0)
        {
            nextScene = self->scenes[self->nextSceneId];
        }

        if (currScene && currScene->onQuit)
        {
            currScene->onQuit(currScene);
        }
        if (nextScene && nextScene->onStart)
        {
            nextScene->onStart(nextScene);
        }
        self->currSceneId = self->nextSceneId;
    }
}

void SceneManager_update(SceneManager* self, float dt)
{
    assert(self && "self must not be NULL");
    assert(self->currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    SceneManager_applySceneChange(self);

    if (self->currSceneId >= 0)
    {
        Scene* currScene = self->scenes[self->currSceneId];
        if (currScene && currScene->onUpdate)
        {
            currScene->onUpdate(currScene, dt);
        }
    }
}

void SceneManager_fixedUpdate(SceneManager* self, float timeStep)
{
    assert(self && "self must not be NULL");
    assert(self->currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    SceneManager_applySceneChange(self);

    if (self->currSceneId >= 0)
    {
        Scene* currScene = self->scenes[self->currSceneId];
        if (currScene && currScene->onUpdate)
        {
            currScene->onFixedUpdate(currScene, timeStep);
        }
    }
}

void SceneManager_render(SceneManager* self)
{
    assert(self && "self must not be NULL");
    assert(self->currSceneId < MAX_SCENE_COUNT && "currSceneId must be within valid range");
    assert(self->nextSceneId < MAX_SCENE_COUNT && "nextSceneId must be within valid range");

    SceneManager_applySceneChange(self);

    if (self->currSceneId >= 0)
    {
        Scene* currScene = self->scenes[self->currSceneId];
        if (currScene && currScene->onRender)
        {
            currScene->onRender(currScene);
        }
    }
}

void SceneManager_addScene(SceneManager* self, void* scenePtr, int sceneId)
{
    assert(self && "self must not be NULL");
    assert(sceneId >= 0 && sceneId < MAX_SCENE_COUNT && "sceneId must be within valid range");
    assert(Scene_isValid(scenePtr) && "scene must be valid");

    Scene* prevScene = self->scenes[sceneId];
    if (prevScene)
    {

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene with ID %d already exists. It will be overwritten.\n", sceneId);
        if (prevScene && prevScene->onDestroy)
        {
            prevScene->onDestroy(prevScene);
        }
        free(prevScene);
    }
    
    Scene* scene = (Scene*)scenePtr;
    self->scenes[sceneId] = scene;
}

void SceneManager_changeScene(SceneManager* self, int sceneId)
{
    assert(self && "self must not be NULL");
    assert(sceneId >= 0 && sceneId < MAX_SCENE_COUNT && "sceneId must be within valid range");
    self->nextSceneId = sceneId;
}

