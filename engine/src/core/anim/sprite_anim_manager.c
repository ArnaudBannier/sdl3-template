/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/anim/sprite_anim_manager.h"

SpriteAnimManager* SpriteAnimManager_create()
{
    SpriteAnimManager* self = (SpriteAnimManager*)calloc(1, sizeof(SpriteAnimManager));
    ASSERT_NEW(self);
    self->entryCount = 0;
    self->entrySize = 1 << 15;
    self->entries = (SpriteAnimEntry**)calloc(self->entrySize, sizeof(SpriteAnimEntry*));
    ASSERT_NEW(self->entries);

    return self;
}

void SpriteAnimManager_destroy(SpriteAnimManager* self)
{
    if (!self) return;
    for (int i = 0; i < self->entrySize; i++)
    {
        SpriteAnimEntry* entry = self->entries[i];
        while (entry)
        {
            SpriteAnimEntry* next = entry->next;
            SpriteAnim_destroy(entry->anim);
            free(entry);
            entry = next;
        }
    }
    free(self->entries);
    free(self);
}

void SpriteAnimManager_addSpriteAnim(SpriteAnimManager* self, AnimId id, SpriteGroup* spriteGroup)
{
    assert(self && "self must not be NULL");
    AnimId index = Uint64_hash((AnimId)id) % self->entrySize;
    SpriteAnimEntry* currEntry = self->entries[index];
    while (currEntry)
    {
        if (currEntry->id == id)
        {
            // L'animation existe déjà
            return;
        }
        currEntry = currEntry->next;
    }

    SpriteAnimEntry* newEntry = (SpriteAnimEntry*)calloc(1, sizeof(SpriteAnimEntry));
    ASSERT_NEW(newEntry);
    newEntry->id = id;
    newEntry->anim = SpriteAnim_create(id, spriteGroup);
    newEntry->next = self->entries[index];

    self->entries[index] = newEntry;
    self->entryCount++;
}

SpriteAnim* SpriteAnimManager_getSpriteAnim(SpriteAnimManager* self, AnimId id)
{
    assert(self && "self must not be NULL");
    AnimId index = Uint64_hash((AnimId)id) % self->entrySize;
    SpriteAnimEntry* currEntry = self->entries[index];
    while (currEntry)
    {
        if (currEntry->id == id)
        {
            return currEntry->anim;
        }
        currEntry = currEntry->next;
    }
    return NULL;
}

void SpriteAnimManager_updateState(const SpriteAnimManager* self, SpriteAnimState* state, float dt)
{
    assert(self && "self must not be NULL");
    AnimId id = state->id;
    AnimId index = Uint64_hash(id) % self->entrySize;
    SpriteAnimEntry* currEntry = self->entries[index];
    while (currEntry)
    {
        if (currEntry->id == id)
        {
            SpriteAnim_updateState(currEntry->anim, state, dt);
            return;
        }
        currEntry = currEntry->next;
    }
}
