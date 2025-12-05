/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/sprite_sheet.h"
#include "core/renderer.h"
#include "game_engine_settings.h"
#include "game_engine_common.h"
#include "cJSON.h"

static void SpriteSheet_parseJSON(SpriteSheet* self, cJSON* root);
static void SpriteSheet_parseRect(SpriteSheet* self, cJSON* jRect, int i);
static void SpriteSheet_parseGeometry(SpriteSheet* self, cJSON* jGeo);
static void SpriteSheet_parseBorder(SpriteSheet* self, cJSON* jBorder);
static void SpriteSheet_parseGroup(SpriteSheet* self, cJSON* jGroup, int i);

SpriteGroup* SpriteGroup_create(SpriteSheet* spriteSheet)
{
    SpriteGroup* self = (SpriteGroup*)calloc(1, sizeof(SpriteGroup));
    AssertNew(self);

    self->m_spriteSheet = spriteSheet;

    return self;
}

void SpriteGroup_destroy(SpriteGroup* self)
{
    if (!self) return;
    if (self->m_name)
    {
        SDL_free(self->m_name);
    }
    free(self);
}

SpriteSheet* SpriteSheet_create(SDL_Texture* texture, const char* desc, Uint64 descLength)
{
    assert(texture && "The texture must be valid");

    SpriteSheet* self = (SpriteSheet*)calloc(1, sizeof(SpriteSheet));
    AssertNew(self);

    self->m_texture = texture;

    cJSON* root = cJSON_ParseWithLength(desc, (size_t)descLength);
    AssertNew(root);

    SpriteSheet_parseJSON(self, root);

    cJSON_Delete(root);
    root = NULL;

    if (self->m_pixelArt)
    {
        bool success = SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        assert(success && "Unable to set texture scale mode");
    }

    return self;
}

void SpriteSheet_destroy(SpriteSheet* self)
{
    if (!self) return;

    if (self->m_texture)
    {
        SDL_DestroyTexture(self->m_texture);
    }
    if (self->m_groups)
    {
        for (int i = 0; i < self->m_groupCount; ++i)
        {
            SpriteGroup_destroy(self->m_groups[i]);
        }
        free(self->m_groups);
    }
    free(self->m_sprites);
}

SpriteGroup* SpriteSheet_getGroupByName(SpriteSheet* self, const char* name)
{
    assert(self && "The SpriteSheet must be valid");
    for (int i = 0; i < self->m_groupCount; ++i)
    {
        if (SDL_strcmp(name, self->m_groups[i]->m_name) == 0)
        {
            return self->m_groups[i];
        }
    }
    return NULL;
}

SpriteGroup* SpriteSheet_getGroupByIndex(SpriteSheet* self, int index)
{
    assert(self && "The SpriteSheet must be valid");
    assert(0 <= index && index < self->m_groupCount);
    return self->m_groups[index];
}

static void SpriteSheet_parseJSON(SpriteSheet* self, cJSON* root)
{
    cJSON* jRects = cJSON_GetObjectItem(root, "rectangles");

    if (cJSON_IsArray(jRects))
    {
        int spriteCount = cJSON_GetArraySize(jRects);
        self->m_spriteCount = spriteCount;
        self->m_sprites = (Sprite*)calloc(spriteCount, sizeof(Sprite));
        AssertNew(self->m_sprites);

        int i = 0;
        cJSON* jRect = NULL;
        cJSON_ArrayForEach(jRect, jRects)
        {
            SpriteSheet_parseRect(self, jRect, i++);
        }
    }
    else
    {
        cJSON* jGeo = cJSON_GetObjectItem(root, "geometry");

        if (jGeo)
        {
            SpriteSheet_parseGeometry(self, jGeo);
        }
        else
        {
            assert(false);
        }
    }

    assert(self->m_sprites && "The sprites must be created");
    assert(self->m_spriteCount > 0 && "The sprite count must be greater than zero");

    cJSON* jBorders = cJSON_GetObjectItem(root, "borders");
    if (cJSON_IsArray(jBorders))
    {
        cJSON* jBorder = NULL;
        cJSON_ArrayForEach(jBorder, jBorders)
        {
            SpriteSheet_parseBorder(self, jBorder);
        }
    }

    cJSON* jPixelArt = cJSON_GetObjectItem(root, "pixelArt");
    if (cJSON_IsBool(jPixelArt))
    {
        self->m_pixelArt = cJSON_IsTrue(jPixelArt);
    }

    cJSON* jGroups = cJSON_GetObjectItem(root, "groups");
    if (cJSON_IsArray(jGroups))
    {
        int groupCount = cJSON_GetArraySize(jGroups);
        self->m_groupCount = groupCount;
        self->m_groups = (SpriteGroup**)calloc(groupCount, sizeof(SpriteGroup*));
        AssertNew(self->m_groups);

        int i = 0;
        cJSON* jGroup = NULL;
        cJSON_ArrayForEach(jGroup, jGroups)
        {
            SpriteSheet_parseGroup(self, jGroup, i++);
        }
    }
}

static void SpriteSheet_parseRect(SpriteSheet* self, cJSON* jRect, int i)
{
    Sprite* sprite = self->m_sprites + i;
    SDL_FRect* rect = &sprite->srcRect;
    cJSON* jTmp = NULL;

    jTmp = cJSON_GetObjectItem(jRect, "x");
    if (cJSON_IsNumber(jTmp))
    {
        rect->x = (float)(jTmp->valueint);
    }

    jTmp = cJSON_GetObjectItem(jRect, "y");
    if (cJSON_IsNumber(jTmp))
    {
        rect->y = (float)(jTmp->valueint);
    }

    jTmp = cJSON_GetObjectItem(jRect, "w");
    if (cJSON_IsNumber(jTmp))
    {
        rect->w = (float)(jTmp->valueint);
    }

    jTmp = cJSON_GetObjectItem(jRect, "h");
    if (cJSON_IsNumber(jTmp))
    {
        rect->h = (float)(jTmp->valueint);
    }
}

static void SpriteSheet_parseGeometry(SpriteSheet* self, cJSON* jGeo)
{
    cJSON* jRow = cJSON_GetObjectItem(jGeo, "rowCount");
    cJSON* jCol = cJSON_GetObjectItem(jGeo, "colCount");
    cJSON* jWidth = cJSON_GetObjectItem(jGeo, "width");
    cJSON* jHeight = cJSON_GetObjectItem(jGeo, "height");
    cJSON* jOffset = cJSON_GetObjectItem(jGeo, "padding");
    cJSON* jBorder = cJSON_GetObjectItem(jGeo, "spacing");
    cJSON* jOffsetX = cJSON_GetObjectItem(jGeo, "paddingX");
    cJSON* jOffsetY = cJSON_GetObjectItem(jGeo, "paddingY");
    cJSON* jBorderX = cJSON_GetObjectItem(jGeo, "spacingX");
    cJSON* jBorderY = cJSON_GetObjectItem(jGeo, "spacingY");

    if (cJSON_IsNumber(jRow) && cJSON_IsNumber(jCol)
        && cJSON_IsNumber(jWidth) && cJSON_IsNumber(jHeight))
    {
        int rowCount = jRow->valueint;
        int colCount = jCol->valueint;
        int w = jWidth->valueint;
        int h = jHeight->valueint;
        int offsetX = cJSON_IsNumber(jOffsetX) ? jOffsetX->valueint : 0;
        int offsetY = cJSON_IsNumber(jOffsetY) ? jOffsetY->valueint : 0;
        int borderX = cJSON_IsNumber(jBorderX) ? jBorderX->valueint : 0;
        int borderY = cJSON_IsNumber(jBorderY) ? jBorderY->valueint : 0;

        if (cJSON_IsNumber(jOffset))
        {
            offsetX = jOffset->valueint;
            offsetY = jOffset->valueint;
        }
        if (cJSON_IsNumber(jBorder))
        {
            borderX = jBorder->valueint;
            borderY = jBorder->valueint;
        }

        int spriteCount = rowCount * colCount;

        self->m_spriteCount = spriteCount;
        self->m_sprites = (Sprite*)calloc(spriteCount, sizeof(Sprite));
        AssertNew(self->m_sprites);

        for (int i = 0; i < rowCount; ++i)
        {
            for (int j = 0; j < colCount; ++j)
            {
                Sprite* sprite = &(self->m_sprites[i * colCount + j]);
                sprite->srcRect.x = (float)(offsetX + j * (w + borderX));
                sprite->srcRect.y = (float)(offsetY + i * (h + borderY));
                sprite->srcRect.w = (float)(w);
                sprite->srcRect.h = (float)(h);
            }
        }
    }
}

static void SpriteSheet_parseBorder(SpriteSheet* self, cJSON* jBorder)
{
    cJSON* jBorderL = cJSON_GetObjectItem(jBorder, "left");
    cJSON* jBorderR = cJSON_GetObjectItem(jBorder, "right");
    cJSON* jBorderU = cJSON_GetObjectItem(jBorder, "top");
    cJSON* jBorderD = cJSON_GetObjectItem(jBorder, "bottom");
    cJSON* jScale = cJSON_GetObjectItem(jBorder, "scale");

    int borderL = cJSON_IsNumber(jBorderL) ? jBorderL->valueint : 0;
    int borderR = cJSON_IsNumber(jBorderR) ? jBorderR->valueint : 0;
    int borderU = cJSON_IsNumber(jBorderU) ? jBorderU->valueint : 0;
    int borderD = cJSON_IsNumber(jBorderD) ? jBorderD->valueint : 0;
    int scale = cJSON_IsNumber(jScale) ? jScale->valueint : 0;

    cJSON* jFrames = cJSON_GetObjectItem(jBorder, "frames");
    if (cJSON_IsArray(jFrames))
    {
        int frameCount = cJSON_GetArraySize(jFrames);
        cJSON* jIdx = NULL;
        cJSON_ArrayForEach(jIdx, jFrames)
        {
            if (!cJSON_IsNumber(jIdx)) continue;

            int spriteIdx = jIdx->valueint;
            assert(0 <= spriteIdx && spriteIdx < self->m_spriteCount);
            Sprite* sprite = self->m_sprites + spriteIdx;
            sprite->hasBorders = true;
            sprite->left = (float)borderL;
            sprite->right = (float)borderR;
            sprite->top = (float)borderU;
            sprite->bottom = (float)borderD;
            sprite->scale = (float)scale;
        }
    }
}

static void SpriteSheet_parseGroup(SpriteSheet* self, cJSON* jGroup, int i)
{
    SpriteGroup* group = SpriteGroup_create(self);
    self->m_groups[i] = group;
    cJSON* jTmp = NULL;

    jTmp = cJSON_GetObjectItem(jGroup, "name");
    if (cJSON_IsString(jTmp))
    {
        group->m_name = SDL_strdup(cJSON_GetStringValue(jTmp));
    }

    jTmp = cJSON_GetObjectItem(jGroup, "frames");
    if (cJSON_IsArray(jTmp))
    {
        int frameCount = cJSON_GetArraySize(jTmp);
        group->m_spriteCount = frameCount;
        group->m_spriteIndices = (int*)calloc(frameCount, sizeof(int));
        AssertNew(group->m_spriteIndices);

        int i = 0;
        cJSON* jIdx = NULL;
        cJSON_ArrayForEach(jIdx, jTmp)
        {
            if (cJSON_IsNumber(jIdx))
            {
                (group->m_spriteIndices)[i] = jIdx->valueint;
            }
            i++;
        }
    }
}

void SpriteGroup_render(SpriteGroup* self, int index, const SDL_FRect* dstRect, Vec2 anchor, float scale)
{
    assert(self && self->m_spriteSheet);
    assert(index >= 0);
    index = index % self->m_spriteCount;
    const SpriteSheet* spriteSheet = self->m_spriteSheet;
    const Sprite* sprite = spriteSheet->m_sprites + self->m_spriteIndices[index];
    const SDL_FRect* srcRect = &(sprite->srcRect);
    bool success = true;

    if (sprite->hasBorders)
    {
        success = RenderTexture9Grid(
            g_renderer, spriteSheet->m_texture, srcRect, dstRect, anchor,
            sprite->left, sprite->right, sprite->top, sprite->bottom, sprite->scale * scale
        );
    }
    else
    {
        success = RenderTexture(
            g_renderer, spriteSheet->m_texture, srcRect, dstRect, anchor
        );
    }
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_render");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}

void SpriteGroup_renderRotated(
    SpriteGroup* self, int index,
    const SDL_FRect* dstRect, Vec2 anchor,
    const double angle, const SDL_FPoint* center, const SDL_FlipMode flip)
{
    assert(self && self->m_spriteSheet);
    assert(index >= 0);
    index = index % self->m_spriteCount;
    const SpriteSheet* spriteSheet = self->m_spriteSheet;
    const Sprite* sprite = spriteSheet->m_sprites + self->m_spriteIndices[index];
    const SDL_FRect* srcRect = &(sprite->srcRect);

    bool success = RenderTextureRotated(
        g_renderer, spriteSheet->m_texture, srcRect, dstRect, anchor,
        angle, flip
    );
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_renderRotated");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}

void SpriteGroup_setOpacity(SpriteGroup* self, Uint8 alpha)
{
    assert(self && self->m_spriteSheet);
    SDL_Texture* texture = self->m_spriteSheet->m_texture;
    bool success = SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_setOpacity");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
    success = SDL_SetTextureAlphaMod(texture, alpha);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_setOpacity");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}

void SpriteGroup_setOpacityFloat(SpriteGroup* self, float alpha)
{
    assert(self && self->m_spriteSheet);
    SDL_Texture* texture = self->m_spriteSheet->m_texture;
    bool success = SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_setOpacityFloat");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
    success = SDL_SetTextureAlphaModFloat(texture, alpha);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_setOpacityFloat");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}

void SpriteGroup_setColorModFloat(SpriteGroup* self, float r, float g, float b)
{
    assert(self && self->m_spriteSheet);
    SDL_Texture* texture = self->m_spriteSheet->m_texture;
    bool success = SDL_SetTextureColorModFloat(texture, r, g, b);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SpriteGroup_setColorModFloat");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}

float SpriteGroup_getAspectRatio(SpriteGroup* self, int index)
{
    assert(self && self->m_spriteSheet);
    assert(index >= 0 && index < self->m_spriteCount);
    const Sprite* sprite = self->m_spriteSheet->m_sprites + self->m_spriteIndices[index];
    return sprite->srcRect.w / sprite->srcRect.h;
}
