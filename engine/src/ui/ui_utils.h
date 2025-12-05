/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"

typedef struct UIRect
{
    Vec2 anchorMin; // Position relative
    Vec2 anchorMax; // Position relative
    Vec2 offsetMin; // Position absolue
    Vec2 offsetMax; // Position absolue
} UIRect;

void UIRect_getAABB(const UIRect* rect, const AABB* parentAABB, AABB* outAABB);
SDL_FRect UIRect_aabbToViewportRect(const AABB* aabb);

typedef struct UITransform
{
    UIRect localRect;
    AABB aabb;
} UITransform;

void UITransform_updateAABB(UITransform* transform, const UITransform* parent);
void UITransform_getViewportRect(const UITransform* transform, SDL_FRect* outRect);
bool UITransform_containsPoint(const UITransform* transform, Vec2 point);
Vec2 UITransform_getSize(const UITransform* transform);
void UITransform_setAbsoluteViewAABB(UITransform* transform, const AABB* aabb);

void UIUtils_renderText(TTF_Text* text, const SDL_FRect* destRect, Vec2 anchor, const SDL_Color* color);

void UIUtils_renderSprite(
    SpriteGroup* spriteGroup, int spriteIndex,
    SDL_Color color, bool useColorMod, SDL_FRect* dstRect);
