/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/game_context.h"
#include "game/game_config.h"

GameContext* GameContext_create()
{
    GameContext* self = (GameContext*)calloc(1, sizeof(GameContext));
    ASSERT_NEW(self);

    AssetManager* assets = AssetManager_create(
        g_engine.sdl.renderer, g_engine.sdl.mixer, g_engine.paths.assets,
        SPRITE_COUNT, FONT_COUNT, AUDIO_COUNT
    );
    SpriteAnimManager* animManager = SpriteAnimManager_create();
    Game_addAssets(assets, animManager);

    AudioSystem* audioSystem = AudioSystem_create(g_engine.sdl.mixer, GAME_AUDIO_GROUP_COUNT);
    Viewport* viewport = Viewport_create();
    UISystem* uiSystem = UISystem_create(assets, audioSystem, viewport, 640.f, MAX_UI_ELEMENT_COUNT);

    self->m_assets = assets;
    self->m_animManager = animManager;
    self->m_viewport = viewport;
    self->m_uiSystem = uiSystem;
    self->m_camera = Camera_create(viewport);
    self->m_input = Input_create(uiSystem);
    self->m_graphicsSystem = GraphicsSystem_create();
    self->m_gizmosSystem = GizmosSystem_create();
    self->m_audioSystem = audioSystem;

    AudioSystem_addGroup(self->m_audioSystem, GAME_AUDIO_GROUP_MUSIC, GAME_MUSIC_TRACK_COUNT);
    AudioSystem_addGroup(self->m_audioSystem, GAME_AUDIO_GROUP_SFX, GAME_SFX_TRACK_COUNT);
    AudioSystem_addGroup(self->m_audioSystem, GAME_AUDIO_GROUP_UI, GAME_UI_TRACK_COUNT);
    UISystem_setAudioGroupIndex(uiSystem, GAME_AUDIO_GROUP_UI);

    AudioSystem_setGain(self->m_audioSystem, GAME_AUDIO_GROUP_SFX, 0.5f);

    AABB worldView = AABB_set(-8.f, -4.5f, +8.f, +4.5f);
    Camera_setWorldView(self->m_camera, worldView);

    const int particleCapacity = 1 << 12;
    self->m_particleSystem = ParticleSystem_create(self->m_animManager, particleCapacity);

    return self;
}

void GameContext_destroy(GameContext* self)
{
    if (!self) return;

    AssetManager_destroy(self->m_assets);
    AudioSystem_destroy(self->m_audioSystem);
    Input_destroy(self->m_input);
    Camera_destroy(self->m_camera);
    Viewport_destroy(self->m_viewport);
    ParticleSystem_destroy(self->m_particleSystem);
    SpriteAnimManager_destroy(self->m_animManager);
    UISystem_destroy(self->m_uiSystem);
    GraphicsSystem_destroy(self->m_graphicsSystem);
    GizmosSystem_destroy(self->m_gizmosSystem);

    free(self);
}

void GameContext_playSpatialSFX(GameContext* self, int audioId, Transform* transform)
{
    assert(self && "self must not be NULL");
    AudioSystem* audioSystem = GameContext_getAudioSystem(self);
    AssetManager* assets = GameContext_getAssetManager(self);
    MIX_Audio* audio = AssetManager_getAudio(assets, audioId);
    AABB worldView = Camera_getWorldView(self->m_camera);
    float pan = (transform->position.x - worldView.lower.x) / (worldView.upper.x - worldView.lower.x);
    pan = 2.f * pan - 1.f;
    pan = Float_clamp(0.7f * pan, -1.f, 1.f);

    AudioPlayOptions options = { 0 };
    AudioPlayOptions_init(&options);
    options.pan = pan;
    options.gain = AssetManager_getAudioGain(assets, audioId);
    options.loopCount = 0;
    options.startTimeMS = 0;

    AudioSystem_play(audioSystem, GAME_AUDIO_GROUP_SFX, audio, &options);
}

void GameContext_playSFX(GameContext* self, int audioId)
{
    assert(self && "self must not be NULL");
    AudioSystem* audioSystem = GameContext_getAudioSystem(self);
    AssetManager* assets = GameContext_getAssetManager(self);
    MIX_Audio* audio = AssetManager_getAudio(assets, audioId);
    AudioSystem_play(audioSystem, GAME_AUDIO_GROUP_SFX, audio, NULL);
}
