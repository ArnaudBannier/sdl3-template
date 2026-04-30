/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "core/sprite_sheet.h"

typedef struct SpriteSheetData SpriteSheetData;
typedef struct FontData FontData;
typedef struct AudioData AudioData;

/// @brief Structure contenant l'ensemble des assets du jeu.
/// Cela correspond aux ressources utilisées (textures, fontes, musiques, son...)
typedef struct AssetManager
{
    SDL_Renderer* m_renderer;
    SDL_Storage* m_storage;
    const char* m_assetsPath;
    MIX_Mixer* m_mixer;

    int m_spriteCapacity;
    SpriteSheetData* m_spriteData;

    int m_fontCapacity;
    FontData* m_fontData;

    int m_audioCapacity;
    AudioData* m_audioData;

    int m_defaultViewportWidth;
} AssetManager;

/// @brief Crée le gestionnaire des assets du jeu.
/// @param spriteCapacity le nombre maximum de sprite sheets pouvant être ajoutées.
/// @param fontCapacity le nombre maximum de polices pouvant être ajoutées.
/// @param audioCapacity le nombre maximum de musiques ou sons pouvant être ajoutés.
/// @return Le gestionnaire des assets du jeu.
AssetManager* AssetManager_create(
    SDL_Renderer* renderer, MIX_Mixer* mixer, const char* assetsPath,
    int spriteCapacity, int fontCapacity, int audioCapacity);

/// @brief Détruit le gestionnaire des assets du jeu.
/// @param self les assets.
void AssetManager_destroy(AssetManager* self);

/// @brief Ajoute une sprite sheet au gestionnaire des assets.
/// @param self le gestionnaire d'assets.
/// @param sheetId l'identifiant de la sprite sheet.
/// 
void AssetManager_addSpriteSheet(
    AssetManager* self, int sheetId, const char* texFileName, const char* descFileName);

/// @brief Ajoute une police au gestionnaire des assets.
/// @param self le gestionnaire d'assets.
/// @param fontId l'identifiant de la police.
/// @param fileName le nom du fichier.
/// @param size la taille de la police.
void AssetManager_addFont(
    AssetManager* self, int fontId, const char* fileName, float size);

void AssetManager_addAudio(
    AssetManager* self, int audioId, const char* fileName, bool predecode);

/// @brief Renvoie une sprite sheet répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param sheetId l'identifiant de la sprite sheet.
/// @return La sprite sheet associée à l'identifiant sheetId;
SpriteSheet* AssetManager_getSpriteSheet(AssetManager* self, int sheetId);

SDL_Texture* AssetManager_getSpriteSheetTexture(AssetManager* self, int sheetId);

/// @brief Renvoie une police répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param fontId l'identifiant de la police.
/// @return Le police associée à l'identifiant fontId.
TTF_Font* AssetManager_getFont(AssetManager* self, int fontId);

MIX_Audio* AssetManager_getAudio(AssetManager* self, int audioId);

/// @brief Charge une sprite sheet répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param sheetId l'identifiant de la sprite sheet.
void AssetManager_cacheSpriteSheet(AssetManager* self, int sheetId);

/// @brief Charge une police répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param fontId l'identifiant de la police.
void AssetManager_cacheFont(AssetManager* self, int fontId);

void AssetManager_cacheAudio(AssetManager* self, int audioId);

void AssetManager_setAudioGain(AssetManager* self, int audioId, float gain);
float AssetManager_getAudioGain(AssetManager* self, int audioId);

INLINE void AssetManager_setDefaultViewportWidth(AssetManager* self, int width)
{
    assert(width > 0 && "The default viewport width must be positive");
    self->m_defaultViewportWidth = width;
}

INLINE int AssetManager_getDefaultViewportWidth(AssetManager* self)
{
    return self->m_defaultViewportWidth;
}

void AssetManager_updateFontSizes(AssetManager* self, float renderScale);

void AssetManager_createIOStream(
    AssetManager* self, const char* fileName,
    SDL_IOStream** outIOStream, void** outBuffer, Uint64* outLength
);
void AssetManager_destroyIOStream(AssetManager* self, SDL_IOStream* ioStream, void* buffer);

void AssetManager_readFileToBuffer(
    AssetManager* self, const char* fileName, void** outBuffer, Uint64* outLength
);

struct SpriteSheetData
{
    SpriteSheet* m_spriteSheet;
    SDL_Texture* m_texture;
    char* m_texFileName;
};

struct FontData
{
    TTF_Font* m_font;
    char* m_fileName;
    void* m_buffer;
    SDL_IOStream* m_ioStream;
    float m_size;
};

struct AudioData
{
    MIX_Audio* m_audio;
    char* m_fileName;
    bool m_predecode;
    float m_gain;
};
