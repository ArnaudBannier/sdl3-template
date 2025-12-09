/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/asset_manager.h"
#include "game_engine_common.h"

static void AssetManager_loadSpriteSheet(AssetManager* self, SpriteSheetData* data);
static void AssetManager_clearSpriteSheet(AssetManager* self, SpriteSheetData* data);

static void AssetManager_loadFont(AssetManager* self, FontData* data);
static void AssetManager_clearFont(AssetManager* self, FontData* data);

static char* AssetManager_makeDatPath(const char* fileName);
SDL_Texture* AssetManager_loadTexture(AssetManager* self, SDL_Renderer* renderer, const char* fileName);

AssetManager* AssetManager_create(int spriteCapacity, int fontCapacity)
{
    AssetManager* self = (AssetManager*)calloc(1, sizeof(AssetManager));
    AssertNew(self);

    self->m_spriteCapacity = spriteCapacity;
    self->m_fontCapacity = fontCapacity;
    self->m_defaultViewportWidth = 1920;

    self->m_spriteData = (SpriteSheetData*)calloc(spriteCapacity, sizeof(SpriteSheetData));
    AssertNew(self->m_spriteData);

    self->m_fontData = (FontData*)calloc(fontCapacity, sizeof(FontData));
    AssertNew(self->m_fontData);

    self->m_storage = SDL_OpenTitleStorage(g_paths.assets, 0);
    AssertNew(self->m_storage);

    return self;
}

void AssetManager_destroy(AssetManager* self)
{
    if (!self) return;

    if (self->m_spriteData)
    {
        // Libère les spriteSheet
        for (int i = 0; i < self->m_spriteCapacity; i++)
        {
            AssetManager_clearSpriteSheet(self, self->m_spriteData + i);
        }
        free(self->m_spriteData);
    }

    if (self->m_fontData)
    {
        // Libère les fonts
        for (int i = 0; i < self->m_fontCapacity; i++)
        {
            AssetManager_clearFont(self, self->m_fontData + i);
        }
        free(self->m_fontData);
    }

    bool success = SDL_CloseStorage(self->m_storage);
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
    AssertNew(fullPath);

    SDL_strlcat(fullPath, fileName, pathSize);
    SDL_strlcat(fullPath, ".dat", pathSize);

    return fullPath;
}

void AssetManager_addSpriteSheet(
    AssetManager* self, int sheetID, const char* texFileName, const char* descFileName)
{
    assert(self && "The AssetManager must be created");
    assert(0 <= sheetID && sheetID < self->m_spriteCapacity && "The sheetID is not valid");

    SpriteSheetData* spriteData = &(self->m_spriteData[sheetID]);
    if (spriteData->m_spriteSheet)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add sprite sheet %s%s", g_paths.assets, texFileName
        );
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The sprite sheet with ID %d already exists", sheetID);
        assert(false && "The sheetID is already used");
        return;
    }
    spriteData->m_texFileName = AssetManager_makeDatPath(texFileName);
    spriteData->m_descFileName = AssetManager_makeDatPath(descFileName);
}

void AssetManager_addFont(
    AssetManager* self, int fontID, const char* fileName, float size)
{
    assert(self && "The AssetManager must be created");
    assert(0 <= fontID && fontID < self->m_fontCapacity && "The fontID is not valid");

    FontData* fontData = &(self->m_fontData[fontID]);
    if (fontData->m_fileName)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add font %s%s", g_paths.assets, fileName
        );
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The font with ID %d already exists", fontID);
        assert(false && "The fontID is already used");
        return;
    }
    fontData->m_fileName = AssetManager_makeDatPath(fileName);
    fontData->m_size = size;
}

SpriteSheet* AssetManager_getSpriteSheet(AssetManager* self, int sheetID)
{
    assert(self && "The AssetManager must be created");
    assert(0 <= sheetID && sheetID < self->m_spriteCapacity && "The sheetID is not valid");

    SpriteSheetData* spriteData = &(self->m_spriteData[sheetID]);
    if (spriteData->m_spriteSheet) return spriteData->m_spriteSheet;

    if (spriteData->m_texFileName == NULL || spriteData->m_descFileName == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No sprite sheet with ID %d", sheetID);
        assert(false && "No sprite sheet with this sheetID");
        return NULL;
    }

    AssetManager_loadSpriteSheet(self, spriteData);
    return spriteData->m_spriteSheet;
}

TTF_Font* AssetManager_getFont(AssetManager* self, int fontID)
{
    assert(self && "The AssetManager must be created");
    assert(0 <= fontID && fontID < self->m_fontCapacity && "The fontID is not valid");

    FontData* fontData = &(self->m_fontData[fontID]);
    if (fontData->m_font) return fontData->m_font;

    if (fontData->m_fileName == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font with ID %d", fontID);
        assert(false && "No font with this fontID");
        return NULL;
    }

    AssetManager_loadFont(self, fontData);
    return fontData->m_font;
}

void AssetManager_cacheSpriteSheet(AssetManager* self, int sheetID)
{
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(self, sheetID);
    assert(spriteSheet);
}

void AssetManager_cacheFont(AssetManager* self, int fontID)
{
    TTF_Font* font = AssetManager_getFont(self, fontID);
    assert(font);
}

static void AssetManager_loadSpriteSheet(AssetManager* self, SpriteSheetData* data)
{
    SDL_Texture* texture = AssetManager_loadTexture(self, g_renderer, data->m_texFileName);

    Uint64 descFileSize = 0;
    void* descBuffer = NULL;
    AssetManager_readFileToBuffer(self, data->m_descFileName, &descBuffer, &descFileSize);

    data->m_spriteSheet = SpriteSheet_create(texture, (char*)descBuffer, (int)descFileSize);
    SDL_free(descBuffer);
}

static void AssetManager_clearSpriteSheet(AssetManager* self, SpriteSheetData* data)
{
    if (data->m_spriteSheet)
    {
        SpriteSheet_destroy(data->m_spriteSheet);
    }
    free(data->m_texFileName);
    free(data->m_descFileName);
    memset(data, 0, sizeof(SpriteSheetData));
}

static void AssetManager_loadFont(AssetManager* self, FontData* data)
{
    Uint64 fileSize = 0;
    AssetManager_createIOStream(self, data->m_fileName, &data->m_ioStream, &data->m_buffer, &fileSize);

    data->m_font = TTF_OpenFontIO(data->m_ioStream, false, data->m_size);
    if (data->m_font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load font %s", data->m_fileName);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        assert(false);
        abort();
    }
}

static void AssetManager_clearFont(AssetManager* self, FontData* data)
{
    if (data->m_font)
    {
        TTF_CloseFont(data->m_font);
        AssetManager_destroyIOStream(self, data->m_ioStream, data->m_buffer);
    }
    free(data->m_fileName);
    memset(data, 0, sizeof(FontData));
}


void AssetManager_readFileToBuffer(
    AssetManager* self, const char* fileName, void** outBuffer, Uint64* outLength)
{
    assert(outBuffer && outLength && "The output buffer and length must be valid");
    Uint64 fileSize = 0;
    bool success = SDL_GetStorageFileSize(self->m_storage, fileName, &fileSize);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to get file size");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
        abort();
    }
    void* mem = SDL_malloc((size_t)fileSize);
    AssertNew(mem);
    success = SDL_ReadStorageFile(self->m_storage, fileName, mem, fileSize);
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

void AssetManager_updateFontSizes(AssetManager* self)
{
    assert(self && "The AssetManager must be created");
    for (int i = 0; i < self->m_fontCapacity; i++)
    {
        FontData* fontData = &(self->m_fontData[i]);
        TTF_Font* font = fontData->m_font;
        if (!font) continue;

        const float newSize = roundf(fontData->m_size * g_sizes.mainRenderScale);
        bool success = TTF_SetFontSize(font, newSize);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to update font size %s", fontData->m_fileName);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
            assert(false);
        }
    }
}
