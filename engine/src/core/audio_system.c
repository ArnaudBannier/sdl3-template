/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/audio_system.h"
#include "engine_common.h"

typedef struct AudioTrackGroup
{
    MIX_Track** tracks;
    size_t trackCount;
    float gain;
} AudioTrackGroup;

AudioSystem* AudioSystem_create(MIX_Mixer* mixer, size_t groupCapacity)
{
    AudioSystem* self = (AudioSystem*)calloc(1, sizeof(AudioSystem));
    ASSERT_NEW(self);

    self->mixer = mixer;
    self->groupCapacity = groupCapacity;
    self->groups = (AudioTrackGroup*)calloc(groupCapacity, sizeof(AudioTrackGroup));
    ASSERT_NEW(self->groups);

    self->playProperties = SDL_CreateProperties();

    return self;
}

static void AudioTrackGroup_destroy(AudioTrackGroup* group)
{
    if (!group) return;
    if (group->tracks)
    {
        for (size_t i = 0; i < group->trackCount; i++)
        {
            MIX_Track* track = group->tracks[i];
            if (!track) continue;
            MIX_DestroyTrack(track);
        }
        free(group->tracks);
    }
}

void AudioSystem_destroy(AudioSystem* self)
{
    if (!self) return;

    if (self->groups)
    {
        for (size_t i = 0; i < self->groupCapacity; i++)
        {
            AudioTrackGroup_destroy(self->groups + i);
        }
        free(self->groups);
    }
    SDL_DestroyProperties(self->playProperties);
    free(self);
}

void AudioSystem_addGroup(AudioSystem* self, size_t groupIndex, size_t trackCount)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    if (group->tracks)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to add audio track group %d", (int)groupIndex);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The audio track group with index %d already exists", (int)groupIndex);
        assert(false && "The group index is already used");
        return;
    }
    group->trackCount = trackCount;
    group->gain = 1.f;
    group->tracks = (MIX_Track**)calloc(trackCount, sizeof(MIX_Track*));
    ASSERT_NEW(group->tracks);

    for (size_t i = 0; i < trackCount; i++)
    {
        group->tracks[i] = MIX_CreateTrack(self->mixer);
        ASSERT_NEW(group->tracks[i]);
    }
}

void AudioSystem_setGain(AudioSystem* self, size_t groupIndex, float gain)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");

    group->gain = gain;
    for (size_t i = 0; i < group->trackCount; i++)
    {
        MIX_Track* track = group->tracks[i];
        bool success = MIX_SetTrackGain(track, gain);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

float AudioSystem_getGain(const AudioSystem* self, size_t groupIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    return group->gain;
}

size_t AudioSystem_getTrackCount(const AudioSystem* self, size_t groupIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    return group->trackCount;
}

MIX_Track* AudioSystem_getTrack(const AudioSystem* self, size_t groupIndex, size_t trackIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");

    AudioTrackGroup* group = self->groups + groupIndex;
    assert(trackIndex < group->trackCount && "The track index is out of bounds");
    return group->tracks[trackIndex];
}

MIX_Track* AudioSystem_aquireTrack(const AudioSystem* self, size_t groupIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");

    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");

    MIX_Track* selectedTrack = NULL;
    for (size_t i = 0; i < group->trackCount; i++)
    {
        MIX_Track* track = group->tracks[i];
        if (!MIX_TrackPlaying(track))
        {
            selectedTrack = track;
            if (!MIX_TrackPaused(track)) return track;
        }
    }
    if (selectedTrack) return selectedTrack;
    return group->tracks[0];
}

void AudioPlayOptions_init(AudioPlayOptions* self)
{
    self->gain = 1.f;
    self->pan = 0.f;
    self->loopCount = 0;
    self->startTimeMS = 0;
}

static void AudioPlayOptions_setTrackPan(const AudioPlayOptions* self, MIX_Track* track)
{
    if (self && self->pan)
    {
        const float pan = Float_clamp(self->pan, -1.f, 1.f);
        const float angle = (pan + 1.0f) * (M_PI / 4.0f);
        const MIX_StereoGains gains = {
            .left = cosf(angle),
            .right = sinf(angle)
        };
        bool success = MIX_SetTrackStereo(track, &gains);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
    else
    {
        bool success = MIX_SetTrackStereo(track, NULL);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

static void AudioPlayOptions_setGain(const AudioPlayOptions* self, MIX_Track* track, float groupGain)
{
    if (self)
    {
        const float gain = self->gain * groupGain;
        bool success = MIX_SetTrackGain(track, gain);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
    else
    {
        bool success = MIX_SetTrackGain(track, groupGain);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

static void AudioPlayOptions_setLoopCount(const AudioPlayOptions* self, SDL_PropertiesID props)
{
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, self ? self->loopCount : 0);
}

void AudioSystem_play(AudioSystem* self, size_t groupIndex, MIX_Audio* audio, const AudioPlayOptions* options)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");

    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");

    MIX_Track* track = AudioSystem_aquireTrack(self, groupIndex);
    AudioPlayOptions_setTrackPan(options, track);
    AudioPlayOptions_setGain(options, track, group->gain);

    SDL_PropertiesID props = self->playProperties;
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, options ? options->loopCount : 0);
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_START_MILLISECOND_NUMBER, options ? options->startTimeMS : 0);

    bool success = MIX_SetTrackAudio(track, audio);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);

    success = MIX_PlayTrack(track, props);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
}

void AudioSystem_playOnTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex, MIX_Audio* audio, const AudioPlayOptions* options)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");

    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");

    assert(trackIndex < group->trackCount && "The track index is out of bounds");
    MIX_Track* track = group->tracks[trackIndex];
    AudioPlayOptions_setTrackPan(options, track);
    AudioPlayOptions_setGain(options, track, group->gain);

    SDL_PropertiesID props = self->playProperties;
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, options ? options->loopCount : 0);
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_START_MILLISECOND_NUMBER, options ? options->startTimeMS : 0);

    bool success = MIX_SetTrackAudio(track, audio);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);

    success = MIX_PlayTrack(track, props);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
}

void AudioSystem_pauseGroup(AudioSystem* self, size_t groupIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    for (size_t i = 0; i < group->trackCount; i++)
    {
        MIX_Track* track = group->tracks[i];
        bool success = MIX_PauseTrack(track);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

void AudioSystem_resumeGroup(AudioSystem* self, size_t groupIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    for (size_t i = 0; i < group->trackCount; i++)
    {
        MIX_Track* track = group->tracks[i];
        bool success = MIX_ResumeTrack(track);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

void AudioSystem_stopGroup(AudioSystem* self, size_t groupIndex, int fadeOutMS)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    for (size_t i = 0; i < group->trackCount; i++)
    {
        MIX_Track* track = group->tracks[i];
        const Sint64 fadeOutFrames = MIX_TrackMSToFrames(track, fadeOutMS);
        bool success = MIX_StopTrack(track, fadeOutFrames);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
    }
}

void AudioSystem_pauseTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    assert(trackIndex < group->trackCount && "The track index is out of bounds");

    MIX_Track* track = group->tracks[trackIndex];
    bool success = MIX_PauseTrack(track);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
}

void AudioSystem_resumeTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    assert(trackIndex < group->trackCount && "The track index is out of bounds");

    MIX_Track* track = group->tracks[trackIndex];
    bool success = MIX_ResumeTrack(track);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
}

void AudioSystem_stopTrack(AudioSystem* self, size_t groupIndex, size_t trackIndex, int fadeOutMS)
{
    assert(self && "self must not be NULL");
    assert(groupIndex < self->groupCapacity && "The group index is out of bounds");
    AudioTrackGroup* group = self->groups + groupIndex;
    assert(group->tracks && "The audio track group does not exist");
    assert(trackIndex < group->trackCount && "The track index is out of bounds");
    MIX_Track* track = group->tracks[trackIndex];
    const Sint64 fadeOutFrames = MIX_TrackMSToFrames(track, fadeOutMS);
    bool success = MIX_StopTrack(track, fadeOutFrames);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_AUDIO);
}
