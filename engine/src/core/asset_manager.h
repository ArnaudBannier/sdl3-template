/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "core/sprite_sheet.h"

typedef struct SpriteSheetData SpriteSheetData;
typedef struct FontData FontData;

/// @brief Structure contenant l'ensemble des assets du jeu.
/// Cela correspond aux ressources utilisées (textures, fontes, musiques, son...)
typedef struct AssetManager
{
    SDL_Storage* m_storage;

    int m_spriteCapacity;
    SpriteSheetData* m_spriteData;

    int m_fontCapacity;
    FontData* m_fontData;

    int m_defaultViewportWidth;
} AssetManager;

/// @brief Crée le gestionnaire des assets du jeu.
/// @param spriteCapacity le nombre maximum de sprite sheets pouvant être ajoutées.
/// @param fontCapacity le nombre maximum de polices pouvant être ajoutées.
/// @return Le gestionnaire des assets du jeu.
AssetManager* AssetManager_create(int spriteCapacity, int fontCapacity);

/// @brief Détruit le gestionnaire des assets du jeu.
/// @param self les assets.
void AssetManager_destroy(AssetManager* self);

/// @brief Ajoute une sprite sheet au gestionnaire des assets.
/// @param self le gestionnaire d'assets.
/// @param sheetID l'identifiant de la sprite sheet.
/// 
void AssetManager_addSpriteSheet(
    AssetManager* self, int sheetID, const char* texFileName, const char* descFileName);

/// @brief Ajoute une police au gestionnaire des assets.
/// @param self le gestionnaire d'assets.
/// @param fontID l'identifiant de la police.
/// @param fileName le nom du fichier.
/// @param size la taille de la police.
void AssetManager_addFont(
    AssetManager* self, int fontID, const char* fileName, float size);

/// @brief Renvoie une sprite sheet répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param sheetID l'identifiant de la sprite sheet.
/// @return La sprite sheet associée à l'identifiant sheetID;
SpriteSheet* AssetManager_getSpriteSheet(AssetManager* self, int sheetID);

/// @brief Renvoie une police répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param fontID l'identifiant de la police.
/// @return Le police associée à l'identifiant fontID.
TTF_Font* AssetManager_getFont(AssetManager* self, int fontID);

/// @brief Charge une sprite sheet répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param sheetID l'identifiant de la sprite sheet.
void AssetManager_cacheSpriteSheet(AssetManager* self, int sheetID);

/// @brief Charge une police répertoriée dans le gestionnaire d'assets.
/// @param self le gestionnaire d'assets.
/// @param fontID l'identifiant de la police.
void AssetManager_cacheFont(AssetManager* self, int fontID);

INLINE void AssetManager_setDefaultViewportWidth(AssetManager* self, int width)
{
    assert(width > 0 && "The default viewport width must be positive");
    self->m_defaultViewportWidth = width;
}

INLINE int AssetManager_getDefaultViewportWidth(AssetManager* self)
{
    return self->m_defaultViewportWidth;
}

void AssetManager_updateFontSizes(AssetManager* self);


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
    char* m_texFileName;
    char* m_descFileName;
};

struct FontData
{
    TTF_Font* m_font;
    char* m_fileName;
    void* m_buffer;
    SDL_IOStream* m_ioStream;
    float m_size;
};
