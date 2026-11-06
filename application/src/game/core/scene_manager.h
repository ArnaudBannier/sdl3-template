/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct GameEngine GameEngine;
typedef struct GameContext GameContext;

typedef struct Scene
{
    uint64_t magicNumber;
    GameEngine* engine;

    void (*onStart)(void* selfPtr);
    void (*onQuit)(void* selfPtr);
    void (*onUpdate)(void* selfPtr, float dt);
    void (*onFixedUpdate)(void* selfPtr, float timeStep);
    void (*onRender)(void* selfPtr);
    void (*onDestroy)(void* selfPtr);
} Scene;

void Scene_init(void* selfPtr, GameEngine* engine);
bool Scene_isValid(void* selfPtr);
GameEngine* Scene_getEngine(void* selfPtr);
GameContext* Scene_getContext(void* selfPtr);

typedef struct SceneManager
{
    GameEngine* engine;
    int currSceneId;
    int nextSceneId;
    Scene* scenes[MAX_SCENE_COUNT];
} SceneManager;

SceneManager* SceneManager_create(GameEngine* engine);
void SceneManager_destroy(SceneManager* self);
void SceneManager_update(SceneManager* self, float dt);
void SceneManager_fixedUpdate(SceneManager* self, float timeStep);
void SceneManager_render(SceneManager* self);
void SceneManager_addScene(SceneManager* self, void* scenePtr, int sceneId);

void SceneManager_changeScene(SceneManager* self, int sceneId);
