/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/graphics_system.h"
#include "ui/ui_system.h"
#include "engine_common.h"

typedef enum GraphicsCmdFlags
{
    GRAPHICS_CMD_TRANSFORM = 1 << 0,
    GRAPHICS_CMD_SORTING_LAYER = 1 << 1,
    GRAPHICS_CMD_BLEND_MOD = 1 << 2,
    GRAPHICS_CMD_COLOR_MOD = 1 << 3,
    GRAPHICS_CMD_TEXT = 1 << 5,
    GRAPHICS_CMD_TEXTURE = 1 << 6,
    GRAPHICS_CMD_RECT = 1 << 7,
    GRAPHICS_CMD_ANCHOR = 1 << 8,
    GRAPHICS_CMD_DIMENSIONS = 1 << 9,
    GRAPHICS_CMD_FLIP = 1 << 10,
} GraphicsCmdFlags;

typedef struct GraphicsCoreCmd
{
    size_t id;
    size_t flags;
    RenderSortingLayer sortingLayer;
    Transform transform;
    RenderAnchor anchor;
    RenderBlendMod blendMod;
    RenderColorMod colorMod;
    RenderDim dimensions;
    RenderFlip flip;
    RenderRect rect;
    RenderTexture texture;
    RenderText text;
} GraphicsCoreCmd;

static void GraphicsSystem_increaseCapacity(GraphicsSystem* self)
{
    size_t newCapacity = self->m_capacity * 2;
    GraphicsCoreCmd* mewCommands = (GraphicsCoreCmd*)realloc(
        self->m_commands,
        newCapacity * sizeof(GraphicsCoreCmd)
    );
    ASSERT_NEW(mewCommands);

    self->m_commands = mewCommands;
    self->m_capacity = newCapacity;
}

GraphicsSystem* GraphicsSystem_create()
{
    GraphicsSystem* self = (GraphicsSystem*)calloc(1, sizeof(GraphicsSystem));
    ASSERT_NEW(self);

    self->m_renderScale = 1.f;
    self->m_capacity = 16;
    self->m_commands = (GraphicsCoreCmd*)calloc(self->m_capacity, sizeof(GraphicsCoreCmd));
    ASSERT_NEW(self->m_commands);

    return self;
}

void GraphicsSystem_destroy(GraphicsSystem* self)
{
    if (!self) return;
    free(self->m_commands);
    free(self);
}

void GraphicsSystem_addCommand(
    GraphicsSystem* self,
    const GraphicsCmd* cmd)
{
    assert(self && "self must not be NULL");
    assert(cmd && "cmd must not be NULL");

    if (self->m_count >= self->m_capacity)
    {
        GraphicsSystem_increaseCapacity(self);
    }
    assert(self->m_count < self->m_capacity && "m_count must be less than m_capacity");

    GraphicsCoreCmd* coreCmd = &self->m_commands[self->m_count];
    memset(coreCmd, 0, sizeof(GraphicsCoreCmd));

    coreCmd->id = self->m_count++;

    if (cmd->sortingLayer)
    {
        coreCmd->flags |= GRAPHICS_CMD_SORTING_LAYER;
        coreCmd->sortingLayer = *(cmd->sortingLayer);
    }
    if (cmd->transform)
    {
        coreCmd->flags |= GRAPHICS_CMD_TRANSFORM;
        coreCmd->transform = *(cmd->transform);
    }
    if (cmd->texture && cmd->texture->texture)
    {
        coreCmd->flags |= GRAPHICS_CMD_TEXTURE;
        coreCmd->texture = *(cmd->texture);
    }
    if (cmd->blendMod)
    {
        coreCmd->flags |= GRAPHICS_CMD_BLEND_MOD;
        coreCmd->blendMod = *(cmd->blendMod);
    }
    if (cmd->colorMod)
    {
        coreCmd->flags |= GRAPHICS_CMD_COLOR_MOD;
        coreCmd->colorMod = *(cmd->colorMod);
    }
    if (cmd->dimensions)
    {
        coreCmd->flags |= GRAPHICS_CMD_DIMENSIONS;
        coreCmd->dimensions = *(cmd->dimensions);
    }
    if (cmd->anchor)
    {
        coreCmd->flags |= GRAPHICS_CMD_ANCHOR;
        coreCmd->anchor = *(cmd->anchor);
    }
    if (cmd->rect)
    {
        coreCmd->flags |= GRAPHICS_CMD_RECT;
        coreCmd->rect = *(cmd->rect);
    }
    if (cmd->flip)
    {
        coreCmd->flags |= GRAPHICS_CMD_FLIP;
        coreCmd->flip = *(cmd->flip);
    }
    if (cmd->text)
    {
        coreCmd->flags |= GRAPHICS_CMD_TEXT;
        coreCmd->text = *(cmd->text);
    }
}

void GraphicsSystem_clear(GraphicsSystem* self)
{
    assert(self && "self must not be NULL");
    self->m_count = 0;
}

static void GraphicsSystem_renderTexture(
    const GraphicsSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GraphicsCoreCmd* coreCmd)
{
    bool success = true;

    SDL_Texture* texture = coreCmd->texture.texture;
    const TextureRegion* region = coreCmd->texture.region;
    const SDL_FRect* srcRect = &(region->srcRect);

    if (coreCmd->flags & GRAPHICS_CMD_COLOR_MOD)
    {
        success = SDL_SetTextureColorModFloat(
            texture,
            coreCmd->colorMod.r,
            coreCmd->colorMod.g,
            coreCmd->colorMod.b
        );
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }
    else
    {
        success = SDL_SetTextureColorModFloat(texture, 1.f, 1.f, 1.f);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }

    if (coreCmd->flags & GRAPHICS_CMD_BLEND_MOD)
    {
        success = SDL_SetTextureBlendMode(texture, coreCmd->blendMod.mode);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        success = SDL_SetTextureAlphaModFloat(texture, coreCmd->blendMod.alpha);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }
    else
    {
        success = SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        success = SDL_SetTextureAlphaModFloat(texture, 1.0f);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }

    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (coreCmd->flags & GRAPHICS_CMD_FLIP)
    {
        flip = coreCmd->flip.mode;
    }

    if ((coreCmd->flags & GRAPHICS_CMD_DIMENSIONS) &&
        (coreCmd->flags & GRAPHICS_CMD_TRANSFORM))
    {
        SDL_FRect dstRect = { 0 };
        switch (coreCmd->transform.space)
        {
        case TRANSFORM_SPACE_WORLD:
            dstRect.x = Camera_worldToViewX(camera, coreCmd->transform.position.x);
            dstRect.y = Camera_worldToViewY(camera, coreCmd->transform.position.y);
            break;
        case TRANSFORM_SPACE_UI:
            dstRect.x = UISystem_uiToViewX(uiSystem, coreCmd->transform.position.x);
            dstRect.y = UISystem_uiToViewY(uiSystem, coreCmd->transform.position.y);
            break;
        default:
            assert(false && "Invalid TransformSpace");
        }

        const float scale = Camera_getWorldToViewScale(camera);
        switch (coreCmd->dimensions.space)
        {
        case RENDER_DIM_SPACE_UI:
            dstRect.w = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.width);
            dstRect.h = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.height);
            break;
        case RENDER_DIM_SPACE_WORLD:
            dstRect.w = Camera_worldToViewLength(camera, coreCmd->dimensions.width);
            dstRect.h = Camera_worldToViewLength(camera, coreCmd->dimensions.height);
            break;
        case RENDER_DIM_SPACE_WORLD_PX_PER_UNITS:
            dstRect.w = Camera_worldToViewLength(camera, srcRect->w / coreCmd->dimensions.width);
            dstRect.h = Camera_worldToViewLength(camera, srcRect->h / coreCmd->dimensions.height);
            break;
        default:
            assert(false && "Invalid RenderDimSpace");
        }

        Vec2 anchor = Vec2_anchor_center;
        if (coreCmd->flags & GRAPHICS_CMD_ANCHOR)
        {
            anchor = coreCmd->anchor.value;
        }
        dstRect.x -= anchor.x * dstRect.w;
        dstRect.y -= (1.f - anchor.y) * dstRect.h;

        if (region->hasBorders)
        {
            const float scale = SDL_roundf(region->scale * self->m_renderScale);
            success = SDL_RenderTexture9Grid(
                renderer, texture, srcRect,
                region->left, region->right, region->top, region->bottom, scale, &dstRect
            );
            CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        }
        else
        {
            SDL_FPoint center = { 0 };
            center.x = anchor.x * dstRect.w;
            center.y = (1.f - anchor.y) * dstRect.h;

            success = SDL_RenderTextureRotated(
                renderer, texture, srcRect, &dstRect,
                -(coreCmd->transform.angle), &center, flip
            );
            CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        }
    }
    else
    {
        success = SDL_RenderTextureRotated(renderer, texture, srcRect, NULL, 0.0, NULL, flip);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }
}

static void GraphicsSystem_renderRect(
    const GraphicsSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GraphicsCoreCmd* coreCmd)
{
    bool success = true;
    float r = coreCmd->rect.color.r / 255.f;
    float g = coreCmd->rect.color.g / 255.f;
    float b = coreCmd->rect.color.b / 255.f;
    float a = coreCmd->rect.color.a / 255.f;

    if (coreCmd->flags & GRAPHICS_CMD_COLOR_MOD)
    {
        r *= coreCmd->colorMod.r;
        g *= coreCmd->colorMod.g;
        b *= coreCmd->colorMod.b;
    }

    if (coreCmd->flags & GRAPHICS_CMD_BLEND_MOD)
    {
        success = SDL_SetRenderDrawBlendMode(renderer, coreCmd->blendMod.mode);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        a *= coreCmd->blendMod.alpha;
    }

    r = Float_clamp01(r) * 255.f;
    g = Float_clamp01(g) * 255.f;
    b = Float_clamp01(b) * 255.f;
    a = Float_clamp01(a) * 255.f;
    success = SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);

    if ((coreCmd->flags & GRAPHICS_CMD_DIMENSIONS) &&
        (coreCmd->flags & GRAPHICS_CMD_TRANSFORM))
    {
        SDL_FRect dstRect = { 0 };
        switch (coreCmd->transform.space)
        {
        case TRANSFORM_SPACE_WORLD:
            dstRect.x = Camera_worldToViewX(camera, coreCmd->transform.position.x);
            dstRect.y = Camera_worldToViewY(camera, coreCmd->transform.position.y);
            break;
        case TRANSFORM_SPACE_UI:
            dstRect.x = UISystem_uiToViewX(uiSystem, coreCmd->transform.position.x);
            dstRect.y = UISystem_uiToViewY(uiSystem, coreCmd->transform.position.y);
            break;
        default:
            assert(false && "Invalid TransformSpace");
        }

        switch (coreCmd->dimensions.space)
        {
        case RENDER_DIM_SPACE_UI:
            dstRect.w = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.width);
            dstRect.h = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.height);
            break;
        case RENDER_DIM_SPACE_WORLD:
            dstRect.w = Camera_worldToViewLength(camera, coreCmd->dimensions.width);
            dstRect.h = Camera_worldToViewLength(camera, coreCmd->dimensions.height);
            break;
        default:
            assert(false && "Invalid RenderDimSpace");
        }

        Vec2 anchor = Vec2_anchor_center;
        if (coreCmd->flags & GRAPHICS_CMD_ANCHOR)
        {
            anchor = coreCmd->anchor.value;
        }
        dstRect.x -= anchor.x * dstRect.w;
        dstRect.y -= (1.f - anchor.y) * dstRect.h;

        if (coreCmd->rect.filled)
        {
            success = SDL_RenderFillRect(renderer, &dstRect);
            CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        }
        else
        {
            success = SDL_RenderRect(renderer, &dstRect);
            CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        }
    }
    else
    {
        success = SDL_RenderFillRect(renderer, NULL);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }
}

static void GraphicsSystem_renderText(
    const GraphicsSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GraphicsCoreCmd* coreCmd)
{
    TTF_Text* ttfText = coreCmd->text.ttfText;
    Vec2 textAnchor = coreCmd->text.anchor;
    bool success = true;
    float r = coreCmd->text.color.r / 255.f;
    float g = coreCmd->text.color.g / 255.f;
    float b = coreCmd->text.color.b / 255.f;
    float a = coreCmd->text.color.a / 255.f;

    if (coreCmd->flags & GRAPHICS_CMD_COLOR_MOD)
    {
        r *= coreCmd->colorMod.r;
        g *= coreCmd->colorMod.g;
        b *= coreCmd->colorMod.b;
    }

    if (coreCmd->flags & GRAPHICS_CMD_BLEND_MOD)
    {
        success = SDL_SetRenderDrawBlendMode(renderer, coreCmd->blendMod.mode);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
        a *= coreCmd->blendMod.alpha;
    }

    r = Float_clamp01(r) * 255.f;
    g = Float_clamp01(g) * 255.f;
    b = Float_clamp01(b) * 255.f;
    a = Float_clamp01(a) * 255.f;
    success = TTF_SetTextColor(ttfText, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);

    int textW = 0;
    int textH = 0;
    success = TTF_GetTextSize(ttfText, &textW, &textH);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);

    SDL_FRect dstRect = { 0 };
    switch (coreCmd->transform.space)
    {
    case TRANSFORM_SPACE_WORLD:
        dstRect.x = Camera_worldToViewX(camera, coreCmd->transform.position.x);
        dstRect.y = Camera_worldToViewY(camera, coreCmd->transform.position.y);
        break;
    case TRANSFORM_SPACE_UI:
        dstRect.x = UISystem_uiToViewX(uiSystem, coreCmd->transform.position.x);
        dstRect.y = UISystem_uiToViewY(uiSystem, coreCmd->transform.position.y);
        break;
    default:
        assert(false && "Invalid TransformSpace");
    }

    switch (coreCmd->dimensions.space)
    {
    case RENDER_DIM_SPACE_UI:
        dstRect.w = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.width);
        dstRect.h = UISystem_uiToViewLength(uiSystem, coreCmd->dimensions.height);
        break;
    case RENDER_DIM_SPACE_WORLD:
        dstRect.w = Camera_worldToViewLength(camera, coreCmd->dimensions.width);
        dstRect.h = Camera_worldToViewLength(camera, coreCmd->dimensions.height);
        break;
    default:
        assert(false && "Invalid RenderDimSpace");
    }

    // Rect anchor
    Vec2 anchor = Vec2_anchor_center;
    if (coreCmd->flags & GRAPHICS_CMD_ANCHOR)
    {
        anchor = coreCmd->anchor.value;
    }
    dstRect.x -= anchor.x * dstRect.w;
    dstRect.y -= (1.f - anchor.y) * dstRect.h;

    // Text anchor
    dstRect.x += textAnchor.x * (dstRect.w - textW);
    dstRect.y += (1.f - textAnchor.y) * (dstRect.h - textH);

    // Pixel perfect
    dstRect.x = roundf(dstRect.x);
    dstRect.y = roundf(dstRect.y);

    success = TTF_DrawRendererText(ttfText, dstRect.x, dstRect.y);
    CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
}

static void GraphicsSystem_renderCmd(
    const GraphicsSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GraphicsCoreCmd* coreCmd)
{
    if (coreCmd->flags & GRAPHICS_CMD_RECT)
    {
        GraphicsSystem_renderRect(self, renderer, camera, uiSystem, coreCmd);
    }
    if (coreCmd->flags & GRAPHICS_CMD_TEXTURE)
    {
        GraphicsSystem_renderTexture(self, renderer, camera, uiSystem, coreCmd);
    }
    if (coreCmd->flags & GRAPHICS_CMD_TEXT)
    {
        GraphicsSystem_renderText(self, renderer, camera, uiSystem, coreCmd);
    }
}

static int GraphicsSystemCmd_compare(const void* ptrA, const void* ptrB)
{
    GraphicsCoreCmd* coreCmdA = (GraphicsCoreCmd*)ptrA;
    GraphicsCoreCmd* coreCmdB = (GraphicsCoreCmd*)ptrB;
    if (coreCmdA->sortingLayer.layer != coreCmdB->sortingLayer.layer)
    {
        return coreCmdA->sortingLayer.layer - coreCmdB->sortingLayer.layer;
    }
    if (coreCmdA->sortingLayer.orderInLayer != coreCmdB->sortingLayer.orderInLayer)
    {
        return coreCmdA->sortingLayer.orderInLayer - coreCmdB->sortingLayer.orderInLayer;
    }
    return (int)coreCmdA->id - (int)coreCmdB->id;
}

void GraphicsSystem_render(
    const GraphicsSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem)
{
    assert(self && "self must not be NULL");
    assert(renderer && "renderer must not be NULL");
    assert(camera && "camera must not be NULL");

    // Sort entries
    qsort(
        self->m_commands,
        self->m_count,
        sizeof(GraphicsCoreCmd),
        GraphicsSystemCmd_compare
    );

    // Render each coreCmd
    for (size_t i = 0; i < self->m_count; i++)
    {
        GraphicsSystem_renderCmd(self, renderer, camera, uiSystem, &self->m_commands[i]);
    }
}
