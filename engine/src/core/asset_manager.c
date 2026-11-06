/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/asset_manager.h"
#include "engine_common.h"

static void AssetManager_clearSpriteSheet(AssetManager* self, SpriteSheetData* data);

static void AssetManager_loadFont(AssetManager* self, FontData* data);
static void AssetManager_clearFont(AssetManager* self, FontData* data);
static void AssetManager_loadAudio(AssetManager* self, AudioData* data);
static void AssetManager_clearAudio(AssetManager* self, AudioData* data);

static char* AssetManager_makeDatPath(const char* fileName);
SDL_Texture* AssetManager_loadTexture(AssetManager* self, SDL_Renderer* renderer, const char* fileName);

AssetManager* AssetManager_create(
    SDL_Renderer* renderer, MIX_Mixer* mixer, const char* assetsPath,
    int spriteCapacity, int fontCapacity, int audioCapacity)
{
    AssetManager* self = (AssetManager*)calloc(1, sizeof(AssetManager));
    ASSERT_NEW(self);

    self->spriteCapacity = spriteCapacity;
    self->fontCapacity = fontCapacity;
    self->audioCapacity = audioCapacity;
    self->defaultViewportWidth = 1920;
    self->renderer = renderer;
    self->mixer = mixer;
    self->assetsPath = assetsPath;

    self->spriteData = (SpriteSheetData*)calloc(spriteCapacity, sizeof(SpriteSheetData));
    ASSERT_NEW(self->spriteData);

    self->fontData = (FontData*)calloc(fontCapacity, sizeof(FontData));
    ASSERT_NEW(self->fontData);

    self->audioData = (AudioData*)calloc(audioCapacity, sizeof(AudioData));
    ASSERT_NEW(self->audioData);

    self->storage = SDL_OpenTitleStorage(assetsPath, 0);
    ASSERT_NEW(self->storage);

    return self;
}

void AssetManager_destroy(AssetManager* self)
{
    if (!self) return;

    if (self->spriteData)
    {
        // Libère les spriteSheet
        for (int i = 0; i < self->spriteCapacity; i++)
        {
            AssetManager_clearSpriteSheet(self, self->spriteData + i);
        }
        free(self->spriteData);
    }

    if (self->fontData)
    {
        // Libère les fonts
        for (int i = 0; i < self->fontCapacity; i++)
        {
            AssetManager_clearFont(self, self->fontData + i);
        }
        free(self->fontData);
    }

    if (self->audioData)
    {
        // Libère les musiques et sons
        for (int i = 0; i < self->audioCapacity; i++)
        {
            AssetManager_clearAudio(self, self->audioData + i);
        }
        free(self->audioData);
    }

    bool success = SDL_CloseStorage(self->storage);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to close storage");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }

    free(self);
}

static char* AssetManager_makeDatPath(const char* fileName)
{
    assert(fileName && "The file name is not valid");

    size_t fileNameLen = strlen(fileName);
    size_t pathSize = fileNameLen + 8;
    char* fullPath = (char*)calloc(pathSize, sizeof(char));
    ASSERT_NEW(fullPath);

    SDL_strlcat(fullPath, fileName, pathSize);
    SDL_strlcat(fullPath, ".dat", pathSize);

    return fullPath;
}

void AssetManager_addSpriteSheet(
    AssetManager* self, int sheetId, const char* texFileName, const char* descFileName)
{
    assert(self && "self must not be NULL");
    assert(0 <= sheetId && sheetId < self->spriteCapacity && "The sheetId is not valid");

    SpriteSheetData* spriteData = &(self->spriteData[sheetId]);
    if (spriteData->spriteSheet)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add sprite sheet %s%s", self->assetsPath, texFileName
        );
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The sprite sheet with ID %d already exists", sheetId);
        assert(false && "The sheetId is already used");
        return;
    }
    spriteData->texFileName = AssetManager_makeDatPath(texFileName);
    char* descDatFileName = AssetManager_makeDatPath(descFileName);

    Uint64 descFileSize = 0;
    void* descBuffer = NULL;
    AssetManager_readFileToBuffer(self, descDatFileName, &descBuffer, &descFileSize);
    free(descDatFileName); descDatFileName = NULL;

    spriteData->spriteSheet = SpriteSheet_create(self, sheetId, (char*)descBuffer, (int)descFileSize);
    SDL_free(descBuffer); descBuffer = NULL;
}

void AssetManager_addFont(
    AssetManager* self, int fontId, const char* fileName, float size)
{
    assert(self && "self must not be NULL");
    assert(0 <= fontId && fontId < self->fontCapacity && "The fontId is not valid");

    FontData* fontData = &(self->fontData[fontId]);
    if (fontData->fileName)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add font %s%s", self->assetsPath, fileName
        );
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The font with ID %d already exists", fontId);
        assert(false && "The fontId is already used");
        return;
    }
    fontData->fileName = AssetManager_makeDatPath(fileName);
    fontData->size = size;
}

void AssetManager_addAudio(AssetManager* self, int audioId, const char* fileName, bool predecode)
{
    assert(self && "self must not be NULL");
    assert(0 <= audioId && audioId < self->audioCapacity && "The audioId is not valid");

    AudioData* audioData = &(self->audioData[audioId]);
    if (audioData->fileName)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add audio %s%s", self->assetsPath, fileName
        );
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The audio with ID %d already exists", audioId);
        assert(false && "The audioId is already used");
        return;
    }
    audioData->fileName = AssetManager_makeDatPath(fileName);
    audioData->predecode = predecode;
}

SpriteSheet* AssetManager_getSpriteSheet(AssetManager* self, int sheetId)
{
    assert(self && "self must not be NULL");
    assert(0 <= sheetId && sheetId < self->spriteCapacity && "The sheetId is not valid");

    SpriteSheetData* spriteData = &(self->spriteData[sheetId]);
    if (spriteData->spriteSheet == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No sprite sheet with ID %d", sheetId);
        assert(false && "No sprite sheet with this sheetId");
        return NULL;
    }

    return spriteData->spriteSheet;
}

SDL_Texture* AssetManager_getSpriteSheetTexture(AssetManager* self, int sheetId)
{
    assert(self && "self must not be NULL");
    assert(0 <= sheetId && sheetId < self->spriteCapacity && "The sheetId is not valid");

    SpriteSheetData* spriteData = &(self->spriteData[sheetId]);
    if (spriteData->texture) return spriteData->texture;

    if (spriteData->texFileName == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No sprite sheet with ID %d", sheetId);
        assert(false && "No sprite sheet with this sheetId");
        return NULL;
    }

    spriteData->texture = AssetManager_loadTexture(self, self->renderer, spriteData->texFileName);
    if (SpriteSheet_isPixelArt(spriteData->spriteSheet))
    {
        bool success = SDL_SetTextureScaleMode(spriteData->texture, SDL_SCALEMODE_NEAREST);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to set texture scale mode");
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
            assert(false);
        }
    }

    return spriteData->texture;
}

TTF_Font* AssetManager_getFont(AssetManager* self, int fontId)
{
    assert(self && "self must not be NULL");
    assert(0 <= fontId && fontId < self->fontCapacity && "The fontId is not valid");

    FontData* fontData = &(self->fontData[fontId]);
    if (fontData->font) return fontData->font;

    if (fontData->fileName == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font with ID %d", fontId);
        assert(false && "No font with this fontId");
        return NULL;
    }

    AssetManager_loadFont(self, fontData);
    return fontData->font;
}

MIX_Audio* AssetManager_getAudio(AssetManager* self, int audioId)
{
    assert(self && "self must not be NULL");
    assert(0 <= audioId && audioId < self->audioCapacity && "The audioId is not valid");

    AudioData* audioData = &(self->audioData[audioId]);
    if (audioData->audio) return audioData->audio;

    if (audioData->fileName == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No audio with ID %d", audioId);
        assert(false && "No audio with this audioId");
        return NULL;
    }

    AssetManager_loadAudio(self, audioData);
    return audioData->audio;
}

void AssetManager_cacheSpriteSheet(AssetManager* self, int sheetId)
{
    SDL_Texture* texture = AssetManager_getSpriteSheetTexture(self, sheetId);
    assert(texture);
}

void AssetManager_cacheFont(AssetManager* self, int fontId)
{
    TTF_Font* font = AssetManager_getFont(self, fontId);
    assert(font);
}

void AssetManager_cacheAudio(AssetManager* self, int audioId)
{
    MIX_Audio* audio = AssetManager_getAudio(self, audioId);
    assert(audio);
}

void AssetManager_setAudioGain(AssetManager* self, int audioId, float gain)
{
    assert(self && "self must not be NULL");
    assert(0 <= audioId && audioId < self->audioCapacity && "The audioId is not valid");

    AudioData* audioData = &(self->audioData[audioId]);
    audioData->gain = gain;
}

float AssetManager_getAudioGain(AssetManager* self, int audioId)
{
    assert(self && "self must not be NULL");
    assert(0 <= audioId && audioId < self->audioCapacity && "The audioId is not valid");
    return self->audioData[audioId].gain;
}

static void AssetManager_clearSpriteSheet(AssetManager* self, SpriteSheetData* data)
{
    if (data->spriteSheet)
    {
        SpriteSheet_destroy(data->spriteSheet);
    }
    if (data->texture)
    {
        SDL_DestroyTexture(data->texture);
    }
    free(data->texFileName);
    memset(data, 0, sizeof(SpriteSheetData));
}

static void AssetManager_loadFont(AssetManager* self, FontData* data)
{
    Uint64 fileSize = 0;
    AssetManager_createIOStream(self, data->fileName, &data->ioStream, &data->buffer, &fileSize);

    data->font = TTF_OpenFontIO(data->ioStream, false, data->size);
    if (data->font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load font %s", data->fileName);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        assert(false);
        abort();
    }
}

static void AssetManager_clearFont(AssetManager* self, FontData* data)
{
    if (data->font)
    {
        TTF_CloseFont(data->font);
        AssetManager_destroyIOStream(self, data->ioStream, data->buffer);
    }
    free(data->fileName);
    memset(data, 0, sizeof(FontData));
}

static void AssetManager_loadAudio(AssetManager* self, AudioData* data)
{
    Uint64 fileSize = 0;
    void* buffer = NULL;
    SDL_IOStream* ioStream = NULL;
    AssetManager_createIOStream(self, data->fileName, &ioStream, &buffer, &fileSize);

    const bool closeIO = false;
    data->audio = MIX_LoadAudio_IO(self->mixer, ioStream, data->predecode, closeIO);
    if (data->audio == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load audio %s", data->fileName);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        assert(false);
        abort();
    }

    AssetManager_destroyIOStream(self, ioStream, buffer);
}

void AssetManager_clearAudio(AssetManager* self, AudioData* data)
{
    if (data->audio)
    {
        MIX_DestroyAudio(data->audio);
    }
    free(data->fileName);
    memset(data, 0, sizeof(AudioData));
}

void AssetManager_readFileToBuffer(
    AssetManager* self, const char* fileName, void** outBuffer, Uint64* outLength)
{
    assert(outBuffer && outLength && "The output buffer and length must be valid");
    Uint64 fileSize = 0;
    bool success = SDL_GetStorageFileSize(self->storage, fileName, &fileSize);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to get file size");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
        abort();
    }
    void* mem = SDL_malloc((size_t)fileSize);
    ASSERT_NEW(mem);
    success = SDL_ReadStorageFile(self->storage, fileName, mem, fileSize);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read file");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
        abort();
    }
    Game_retriveMem(mem, (size_t)fileSize);
    *outBuffer = mem;
    *outLength = fileSize;
}

void AssetManager_createIOStream(
    AssetManager* self, const char* fileName,
    SDL_IOStream** outIOStream, void** outBuffer, Uint64* outLength)
{
    Uint64 fileSize = 0;
    AssetManager_readFileToBuffer(self, fileName, outBuffer, outLength);

    *outIOStream = SDL_IOFromConstMem(*outBuffer, (size_t)(*outLength));
    if (*outIOStream == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create IOStream %s", fileName);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        assert(false);
        abort();
    }
}

void AssetManager_destroyIOStream(AssetManager* self, SDL_IOStream* ioStream, void* buffer)
{
    if (ioStream) SDL_CloseIO(ioStream);
    SDL_free(buffer);
}

SDL_Texture* AssetManager_loadTexture(AssetManager* self, SDL_Renderer* renderer, const char* fileName)
{
    Uint64 fileSize = 0;
    void* buffer = NULL;
    SDL_IOStream* ioStream = NULL;
    AssetManager_createIOStream(self, fileName, &ioStream, &buffer, &fileSize);

    SDL_Texture* texPtr = IMG_LoadTexture_IO(renderer, ioStream, false);
    if (texPtr == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load texture %s", fileName);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        assert(false);
        abort();
    }

    AssetManager_destroyIOStream(self, ioStream, buffer);

    return texPtr;
}

void AssetManager_updateFontSizes(AssetManager* self, float renderScale)
{
    assert(self && "self must not be NULL");
    for (int i = 0; i < self->fontCapacity; i++)
    {
        FontData* fontData = &(self->fontData[i]);
        TTF_Font* font = fontData->font;
        if (!font) continue;

        const float newSize = roundf(fontData->size * renderScale);
        bool success = TTF_SetFontSize(font, newSize);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to update font size %s", fontData->fileName);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
            assert(false);
        }
    }
}
