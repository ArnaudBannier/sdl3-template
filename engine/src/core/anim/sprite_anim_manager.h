/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/anim/sprite_anim.h"
#include "core/anim/sprite_anim_state.h"

typedef struct SpriteAnimEntry SpriteAnimEntry;
struct SpriteAnimEntry
{
    AnimId id;
    SpriteAnim* anim;
    SpriteAnimEntry* next;
};

typedef struct SpriteAnimManager
{
    int tmp;
    SpriteAnimEntry** m_entries;
    int m_entryCount;
    int m_entrySize;
} SpriteAnimManager;

SpriteAnimManager* SpriteAnimManager_create();
void SpriteAnimManager_destroy(SpriteAnimManager* self);

void SpriteAnimManager_addSpriteAnim(
    SpriteAnimManager* self, AnimId id, SpriteGroup* spriteGroup
);

SpriteAnim* SpriteAnimManager_getSpriteAnim(
    SpriteAnimManager* self, AnimId id
);

void SpriteAnimManager_updateState(const SpriteAnimManager* self, SpriteAnimState* state, float dt);
