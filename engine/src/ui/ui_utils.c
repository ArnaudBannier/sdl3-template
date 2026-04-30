/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_utils.h"
#include "ui/ui_system.h"
#include "engine_common.h"

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

SDL_FRect UIRect_aabbToViewportRect(const AABB* aabb, const UISystem* uiSystem)
{
    Vec2 size = AABB_getSize(aabb);
    SDL_FRect rect = { 0 };
    rect.x = UISystem_uiToViewX(uiSystem, aabb->lower.x);
    rect.y = UISystem_uiToViewY(uiSystem, aabb->upper.y);
    rect.w = UISystem_uiToViewLength(uiSystem, size.x);
    rect.h = UISystem_uiToViewLength(uiSystem, size.y);
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

void UITransform_getComponents(
    const UITransform* self,
    Transform* transform, RenderDim* dim, RenderAnchor* anchor)
{
    assert(self && "self must not be NULL");
    assert(transform && "transform must not be NULL");
    assert(dim && "dim must not be NULL");
    assert(anchor && "anchor must not be NULL");
    Vec2 size = AABB_getSize(&self->aabb);

    transform->space = TRANSFORM_SPACE_UI;
    transform->position = self->aabb.lower;

    dim->space = RENDER_DIM_SPACE_UI;
    dim->width = size.x;
    dim->height = size.y;

    anchor->value = Vec2_anchor_south_west;
}

void UITransform_getGizmos(const UITransform* self, Transform* transform, GizmosRect* rect)
{
    assert(self && "self must not be NULL");
    assert(transform && "transform must not be NULL");
    assert(rect && "rect must not be NULL");
    Vec2 size = AABB_getSize(&self->aabb);
    transform->space = TRANSFORM_SPACE_UI;
    transform->position = self->aabb.lower;
    rect->anchor = Vec2_anchor_south_west;
    rect->width = size.x;
    rect->height = size.y;
    rect->filled = false;
}

void UITransform_getViewportRect(const UITransform* transform, const UISystem* uiSystem, SDL_FRect* outRect)
{
    assert(transform && "transform must not be NULL");
    assert(uiSystem && "uiSystem must not be NULL");
    assert(outRect && "outRect must not be NULL");
    const AABB* aabb = &transform->aabb;
    Vec2 size = AABB_getSize(aabb);
    SDL_FRect rect = { 0 };
    rect.x = UISystem_uiToViewX(uiSystem, aabb->lower.x);
    rect.y = UISystem_uiToViewY(uiSystem, aabb->upper.y);
    rect.w = UISystem_uiToViewLength(uiSystem, size.x);
    rect.h = UISystem_uiToViewLength(uiSystem, size.y);
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
    Vec2 pixelsPerUnit = g_engine.sizes.uiPixelsPerUnit;
    transform->localRect.offsetMin = Vec2_div(aabb->lower, pixelsPerUnit);
    transform->localRect.offsetMax = Vec2_div(aabb->upper, pixelsPerUnit);
}
