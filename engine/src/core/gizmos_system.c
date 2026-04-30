/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/gizmos_system.h"
#include "ui/ui_system.h"
#include "engine_common.h"

typedef enum GizmosCmdFlags
{
    GIZMOS_CMD_TRANSFORM = 1 << 0,
    GIZMOS_CMD_COLOR = 1 << 1,
    GIZMOS_CMD_CIRCLE = 1 << 2,
    GIZMOS_CMD_RECT = 1 << 3,
    GIZMOS_CMD_BLEND_MOD = 1 << 4,
} GizmosCmdFlags;

typedef struct GizmosCoreCmd
{
    size_t flags;
    Transform transform;
    SDL_Color color;
    RenderBlendMod blendMod;
    GizmosCircle circle;
    GizmosRect rect;
} GizmosCoreCmd;

static void GizmosSystem_increaseCapacity(GizmosSystem* self)
{
    size_t newCapacity = self->m_capacity * 2;
    GizmosCoreCmd* mewCommands = (GizmosCoreCmd*)realloc(
        self->m_commands,
        newCapacity * sizeof(GizmosCoreCmd)
    );
    ASSERT_NEW(mewCommands);

    self->m_commands = mewCommands;
    self->m_capacity = newCapacity;
}

GizmosSystem* GizmosSystem_create()
{
    GizmosSystem* self = (GizmosSystem*)calloc(1, sizeof(GizmosSystem));
    ASSERT_NEW(self);

    self->m_capacity = 16;
    self->m_commands = (GizmosCoreCmd*)calloc(self->m_capacity, sizeof(GizmosCoreCmd));
    ASSERT_NEW(self->m_commands);

    return self;
}

void GizmosSystem_destroy(GizmosSystem* self)
{
    if (!self) return;
    free(self->m_commands);
    free(self);
}

void GizmosSystem_addCommand(GizmosSystem* self, const GizmosCmd* cmd)
{
    assert(self && "self must not be NULL");
    assert(cmd && "cmd must not be NULL");

    if (self->m_count >= self->m_capacity)
    {
        GizmosSystem_increaseCapacity(self);
    }
    assert(self->m_count < self->m_capacity && "m_count must be less than m_capacity");

    GizmosCoreCmd* coreCmd = &self->m_commands[self->m_count];
    memset(coreCmd, 0, sizeof(GizmosCoreCmd));
    self->m_count++;

    if (cmd->transform)
    {
        coreCmd->flags |= GIZMOS_CMD_TRANSFORM;
        coreCmd->transform = *(cmd->transform);
    }
    if (cmd->color)
    {
        coreCmd->flags |= GIZMOS_CMD_COLOR;
        coreCmd->color = *(cmd->color);
    }
    if (cmd->blendMod)
    {
        coreCmd->flags |= GIZMOS_CMD_BLEND_MOD;
        coreCmd->blendMod = *(cmd->blendMod);
    }
    if (cmd->circle)
    {
        coreCmd->flags |= GIZMOS_CMD_CIRCLE;
        coreCmd->circle = *(cmd->circle);
    }
    if (cmd->rect)
    {
        coreCmd->flags |= GIZMOS_CMD_RECT;
        coreCmd->rect = *(cmd->rect);
    }
}

void GizmosSystem_clear(GizmosSystem* self)
{
    assert(self && "self must not be NULL");
    self->m_count = 0;
}

static void GizmosSystem_renderCircle(
    const GizmosSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GizmosCoreCmd* coreCmd)
{
    bool success = true;
    float r = 1.f;
    float g = 1.f;
    float b = 1.f;
    float a = 1.f;

    if (coreCmd->flags & GIZMOS_CMD_COLOR)
    {
        r *= coreCmd->color.r / 255.f;
        g *= coreCmd->color.g / 255.f;
        b *= coreCmd->color.b / 255.f;
        a *= coreCmd->color.a / 255.f;
    }
    if (coreCmd->flags & GIZMOS_CMD_BLEND_MOD)
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

    if (coreCmd->flags & GIZMOS_CMD_TRANSFORM)
    {
        Vec2 vertices[17] = { 0 };
        SDL_FPoint points[17] = { 0 };
        for (int i = 0; i < 17; i++)
        {
            float angle = (float)i / 16.f * 2.f * M_PI;
            const Vec2 p = Vec2_add(
                coreCmd->transform.position,
                Vec2_scale(Vec2_set(cosf(angle), sinf(angle)), coreCmd->circle.radius)
            );

            switch (coreCmd->transform.space)
            {
            case TRANSFORM_SPACE_WORLD:
                Camera_worldToView(camera, p, &(points[i].x), &(points[i].y));
                break;
            case TRANSFORM_SPACE_UI:
                UISystem_uiToView(uiSystem, p, &(points[i].x), &(points[i].y));
                break;
            default:
                assert(false && "Invalid TransformSpace");
            }
        }
        success = SDL_RenderLines(renderer, points, 17);
        CHECK_SDL_SUCCESS(success, SDL_LOG_CATEGORY_RENDER);
    }
}

static void GizmosSystem_renderRect(
    const GizmosSystem* self, SDL_Renderer* renderer,
    const Camera* camera, const UISystem* uiSystem,
    const GizmosCoreCmd* coreCmd)
{
    bool success = true;
    float r = 1.f;
    float g = 1.f;
    float b = 1.f;
    float a = 1.f;

    if (coreCmd->flags & GIZMOS_CMD_COLOR)
    {
        r *= coreCmd->color.r / 255.f;
        g *= coreCmd->color.g / 255.f;
        b *= coreCmd->color.b / 255.f;
        a *= coreCmd->color.a / 255.f;
    }
    if (coreCmd->flags & GIZMOS_CMD_BLEND_MOD)
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

    if (coreCmd->flags & GIZMOS_CMD_TRANSFORM)
    {
        SDL_FRect dstRect = { 0 };
        switch (coreCmd->transform.space)
        {
        case TRANSFORM_SPACE_WORLD:
            dstRect.x = Camera_worldToViewX(camera, coreCmd->transform.position.x);
            dstRect.y = Camera_worldToViewY(camera, coreCmd->transform.position.y);
            dstRect.w = Camera_worldToViewLength(camera, coreCmd->rect.width);
            dstRect.h = Camera_worldToViewLength(camera, coreCmd->rect.height);
            break;
        case TRANSFORM_SPACE_UI:
            dstRect.x = UISystem_uiToViewX(uiSystem, coreCmd->transform.position.x);
            dstRect.y = UISystem_uiToViewY(uiSystem, coreCmd->transform.position.y);
            dstRect.w = UISystem_uiToViewLength(uiSystem, coreCmd->rect.width);
            dstRect.h = UISystem_uiToViewLength(uiSystem, coreCmd->rect.height);
            break;
        default:
            assert(false && "Invalid TransformSpace");
        }

        Vec2 anchor = coreCmd->rect.anchor;
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
}

void GizmosSystem_render(
    const GizmosSystem* self,
    SDL_Renderer* renderer,
    const Camera* camera,
    const UISystem* uiSystem)
{
    for (size_t i = 0; i < self->m_count; i++)
    {
        GizmosCoreCmd* coreCmd = &self->m_commands[i];
        if (coreCmd->flags & GIZMOS_CMD_CIRCLE)
        {
            GizmosSystem_renderCircle(self, renderer, camera, uiSystem, coreCmd);
        }
        if (coreCmd->flags & GIZMOS_CMD_RECT)
        {
            GizmosSystem_renderRect(self, renderer, camera, uiSystem, coreCmd);
        }
    }
}
