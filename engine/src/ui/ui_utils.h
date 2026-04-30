/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"
#include "core/components.h"
#include "core/graphics_system.h"
#include "core/gizmos_system.h"

typedef struct UISystem UISystem;

typedef struct UIRect
{
    Vec2 anchorMin; // Position relative
    Vec2 anchorMax; // Position relative
    Vec2 offsetMin; // Position absolue
    Vec2 offsetMax; // Position absolue
} UIRect;

void UIRect_getAABB(const UIRect* rect, const AABB* parentAABB, AABB* outAABB);
SDL_FRect UIRect_aabbToViewportRect(const AABB* aabb, const UISystem* uiSystem);

typedef struct UITransform
{
    UIRect localRect;
    AABB aabb;
} UITransform;

void UITransform_updateAABB(UITransform* transform, const UITransform* parent);
void UITransform_getComponents(const UITransform* self, Transform* transform, RenderDim* dim, RenderAnchor* anchor);
void UITransform_getGizmos(const UITransform* self, Transform* transform, GizmosRect* rect);
void UITransform_getViewportRect(const UITransform* transform, const UISystem* uiSystem, SDL_FRect* outRect);
bool UITransform_containsPoint(const UITransform* transform, Vec2 point);
Vec2 UITransform_getSize(const UITransform* transform);
void UITransform_setAbsoluteViewAABB(UITransform* transform, const AABB* aabb);
