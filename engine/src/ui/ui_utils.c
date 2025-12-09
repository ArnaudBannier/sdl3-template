/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_utils.h"
#include "game_engine_common.h"

void UIRect_getAABB(const UIRect* rect, const AABB* parentAABB, AABB* outAABB)
{
    if (parentAABB)
    {
        Vec2 pSize = AABB_getSize(parentAABB);
        outAABB->lower.x = parentAABB->lower.x + pSize.x * rect->anchorMin.x + rect->offsetMin.x;
        outAABB->lower.y = parentAABB->lower.y + pSize.y * rect->anchorMin.y + rect->offsetMin.y;
        outAABB->upper.x = parentAABB->lower.x + pSize.x * rect->anchorMax.x + rect->offsetMax.x;
        outAABB->upper.y = parentAABB->lower.y + pSize.y * rect->anchorMax.y + rect->offsetMax.y;
    }
    else
    {
        outAABB->lower = rect->offsetMin;
        outAABB->upper = rect->offsetMax;
    }
}

SDL_FRect UIRect_aabbToViewportRect(const AABB* aabb)
{
    Vec2 pixelsPerUnit = g_sizes.uiPixelsPerUnit;
    Vec2 size = AABB_getSize(aabb);
    SDL_FRect rect = { 0 };
    rect.x = aabb->lower.x * pixelsPerUnit.x;
    rect.y = (g_sizes.uiSize.y - aabb->upper.y) * pixelsPerUnit.y;
    rect.w = size.x * pixelsPerUnit.x;
    rect.h = size.y * pixelsPerUnit.y;
    return rect;
}

void UITransform_updateAABB(UITransform* transform, const UITransform* parent)
{
    assert(transform && "transform must not be NULL");
    AABB* aabb = &transform->aabb;
    const UIRect* rect = &transform->localRect;
    if (parent)
    {
        const AABB* pAABB = &parent->aabb;
        Vec2 pSize = AABB_getSize(pAABB);
        aabb->lower.x = pAABB->lower.x + pSize.x * rect->anchorMin.x + rect->offsetMin.x;
        aabb->lower.y = pAABB->lower.y + pSize.y * rect->anchorMin.y + rect->offsetMin.y;
        aabb->upper.x = pAABB->lower.x + pSize.x * rect->anchorMax.x + rect->offsetMax.x;
        aabb->upper.y = pAABB->lower.y + pSize.y * rect->anchorMax.y + rect->offsetMax.y;
    }
    else
    {
        UIRect_getAABB(&transform->localRect, NULL, &transform->aabb);
        aabb->lower = transform->localRect.offsetMin;
        aabb->upper = transform->localRect.offsetMax;
    }
}

void UITransform_getViewportRect(const UITransform* transform, SDL_FRect* outRect)
{
    assert(transform && "transform must not be NULL");
    assert(outRect && "outRect must not be NULL");

    Vec2 pixelsPerUnit = g_sizes.uiPixelsPerUnit;
    const AABB* aabb = &transform->aabb;
    Vec2 size = AABB_getSize(aabb);
    SDL_FRect rect = { 0 };
    rect.x = aabb->lower.x * pixelsPerUnit.x;
    rect.y = (g_sizes.uiSize.y - aabb->upper.y) * pixelsPerUnit.y;
    rect.w = size.x * pixelsPerUnit.x;
    rect.h = size.y * pixelsPerUnit.y;
    *outRect = rect;
}

bool UITransform_containsPoint(const UITransform* transform, Vec2 point)
{
    return AABB_containsPoint(&transform->aabb, point);
}

Vec2 UITransform_getSize(const UITransform* transform)
{
    return Vec2_sub(transform->aabb.upper, transform->aabb.lower);
}

void UITransform_setAbsoluteViewAABB(UITransform* transform, const AABB* aabb)
{
    Vec2 pixelsPerUnit = g_sizes.uiPixelsPerUnit;
    transform->localRect.offsetMin = Vec2_div(aabb->lower, pixelsPerUnit);
    transform->localRect.offsetMax = Vec2_div(aabb->upper, pixelsPerUnit);
}

void UIUtils_renderText(TTF_Text* text, const SDL_FRect* destRect, Vec2 anchor, const SDL_Color* color)
{
    bool success = true;
    success = TTF_SetTextColor(text, color->r, color->g, color->b, color->a);
    assert(success);

    int textW = 0;
    int textH = 0;
    success = TTF_GetTextSize(text, &textW, &textH);
    assert(success);

    SDL_FRect textRect = *destRect;
    textRect.x += anchor.x * (textRect.w - textW);
    textRect.y += (1.f - anchor.y) * (textRect.h - textH);
    textRect.x = roundf(textRect.x);
    textRect.y = roundf(textRect.y);

    success = TTF_DrawRendererText(text, textRect.x, textRect.y);
    assert(success);
}

void UIUtils_renderSprite(
    SpriteGroup* spriteGroup, int spriteIndex,
    SDL_Color color, bool useColorMod, SDL_FRect* dstRect)
{
    assert(dstRect && "dstRect must not be NULL");
    bool success = true;

    if (spriteGroup && spriteIndex >= 0)
    {
        if (useColorMod)
        {
            float a = color.a / 255.f;
            float r = color.r / 255.f * a;
            float g = color.g / 255.f * a;
            float b = color.b / 255.f * a;

            SpriteGroup_setColorModFloat(spriteGroup, r, g, b);
        }
        else
        {
            SpriteGroup_setColorModFloat(spriteGroup, 1.f, 1.f, 1.f);
        }

        SpriteGroup_render(spriteGroup, spriteIndex, dstRect, Vec2_anchor_north_west, g_sizes.mainRenderScale);
    }
    else
    {
        success = SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "UIUtils_renderSprite");
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
            assert(false);
        }

        success = SDL_RenderFillRect(g_renderer, dstRect);
        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "UIUtils_renderSprite");
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
            assert(false);
        }
    }
}
