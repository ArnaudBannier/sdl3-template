/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/assets.h"

typedef struct SheetLoadInfo
{
    int sheetId;
    const char* texPath;
    const char* descPath;
} SheetLoadInfo;

typedef struct FontLoadInfo
{
    GameFontId fontId;
    const char* path;
    float size;
} FontLoadInfo;

typedef struct AudioLoadInfo
{
    GameAudioId audioId;
    const char* path;
    float gain;
    bool predecode;
} AudioLoadInfo;

typedef struct AnimLoadInfo
{
    AnimId animId;
    int sheetId;
    const char* groupName;
    float fps;
    int loopCount;
} AnimLoadInfo;

void Game_addAssets(AssetManager* assets, SpriteAnimManager* animManager)
{
    // -------------------------------------------------------------------------
    // Sprite sheets

    static const SheetLoadInfo sheetsToLoad[] = {
        { SPRITE_UI_BASE, "atlas/ui_base", "atlas/ui_base_desc" },
    };
    static const int sheetCount = sizeof(sheetsToLoad) / sizeof(SheetLoadInfo);

    for (int i = 0; i < sheetCount; i++)
    {
        AssetManager_addSpriteSheet(
            assets,
            sheetsToLoad[i].sheetId,
            sheetsToLoad[i].texPath,
            sheetsToLoad[i].descPath
        );
    }

    // -------------------------------------------------------------------------
    // Audios

    static const AudioLoadInfo audiosToLoad[] = {
        // UI
        { AUDIO_UI_CANCEL, "audio/sound_fx/ui_cancel", 1.f, true },
        { AUDIO_UI_PAUSE, "audio/sound_fx/ui_pause", 1.f, true },
        { AUDIO_UI_SELECT, "audio/sound_fx/ui_select", 1.f, true },
        { AUDIO_UI_VALIDATE, "audio/sound_fx/ui_validate", 1.f, true },
    };
    static const int audioCount = sizeof(audiosToLoad) / sizeof(AudioLoadInfo);

    for (int i = 0; i < audioCount; i++)
    {
        AssetManager_addAudio(
            assets,
            audiosToLoad[i].audioId,
            audiosToLoad[i].path,
            audiosToLoad[i].predecode
        );
        AssetManager_setAudioGain(
            assets,
            audiosToLoad[i].audioId,
            audiosToLoad[i].gain
        );
    }

    // -------------------------------------------------------------------------
    // Fonts

    static const FontLoadInfo fontsToLoad[] = {
        { FONT_NORMAL, "font/noto_sans_regular", 32 },
        { FONT_BOLD, "font/noto_sans_bold", 32 },
        { FONT_BIG, "font/noto_sans_regular", 64 },
        { FONT_BIG_BOLD, "font/noto_sans_bold", 64 },
        { FONT_TITLE, "font/noto_sans_bold", 96 },
    };
    static const int fontCount = sizeof(fontsToLoad) / sizeof(FontLoadInfo);

    for (int i = 0; i < fontCount; i++)
    {
        AssetManager_addFont(
            assets,
            fontsToLoad[i].fontId,
            fontsToLoad[i].path,
            fontsToLoad[i].size
        );
    }

    // -------------------------------------------------------------------------
    // Animations

    //static const AnimLoadInfo animsToLoad[] = {};
    //static const int animCount = sizeof(animsToLoad) / sizeof(AnimLoadInfo);

    //for (int i = 0; i < animCount; i++)
    //{
    //    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(
    //        assets,
    //        animsToLoad[i].sheetId
    //    );
    //    ASSERT_NEW(spriteSheet);
    //    SpriteGroup* spriteGroup = SpriteSheet_getGroupByName(
    //        spriteSheet,
    //        animsToLoad[i].groupName
    //    );
    //    ASSERT_NEW(spriteGroup);

    //    SpriteAnimManager_addSpriteAnim(
    //        animManager,
    //        animsToLoad[i].animId,
    //        spriteGroup
    //    );
    //    SpriteAnim* anim = SpriteAnimManager_getSpriteAnim(
    //        animManager,
    //        animsToLoad[i].animId
    //    );
    //    ASSERT_NEW(anim);
    //    SpriteAnim_setFPS(anim, animsToLoad[i].fps);
    //    SpriteAnim_setCycleCount(anim, animsToLoad[i].loopCount);
    //}
}
