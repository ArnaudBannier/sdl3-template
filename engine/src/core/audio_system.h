/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"

typedef struct AudioTrackGroup AudioTrackGroup;

typedef struct AudioSystem
{
    MIX_Mixer* m_mixer;
    AudioTrackGroup* m_groups;
    size_t m_groupCapacity;
    SDL_PropertiesID m_playProperties;
} AudioSystem;

AudioSystem* AudioSystem_create(MIX_Mixer* mixer, size_t groupCapacity);
void AudioSystem_destroy(AudioSystem* self);
void AudioSystem_addGroup(AudioSystem* self, size_t groupIndex, size_t trackCount);
void AudioSystem_setGain(AudioSystem* self, size_t groupIndex, float gain);
float AudioSystem_getGain(const AudioSystem* self, size_t groupIndex);
size_t AudioSystem_getTrackCount(const AudioSystem* self, size_t groupIndex);
MIX_Track* AudioSystem_getTrack(const AudioSystem* self, size_t groupIndex, size_t trackIndex);
MIX_Track* AudioSystem_aquireTrack(const AudioSystem* self, size_t groupIndex);

typedef struct AudioPlayOptions
{
    int loopCount;
    int startTimeMS;
    float pan;
    float gain;
} AudioPlayOptions;

void AudioPlayOptions_init(AudioPlayOptions* self);

void AudioSystem_play(AudioSystem* self, size_t groupIndex, MIX_Audio* audio, const AudioPlayOptions* options);
void AudioSystem_playOnTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex, MIX_Audio* audio, const AudioPlayOptions* options);

void AudioSystem_pauseGroup(AudioSystem* self, size_t groupIndex);
void AudioSystem_resumeGroup(AudioSystem* self, size_t groupIndex);
void AudioSystem_stopGroup(AudioSystem* self, size_t groupIndex, int fadeOutMS);

void AudioSystem_pauseTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex);
void AudioSystem_resumeTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex);
void AudioSystem_stopTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex, int fadeOutMS);
