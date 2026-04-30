/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/camera.h"
#include "core/components.h"

typedef struct UISystem UISystem;
typedef struct GizmosCoreCmd GizmosCoreCmd;

typedef struct GizmosSystem
{
    GizmosCoreCmd* m_commands;
    size_t m_count;
    size_t m_capacity;
} GizmosSystem;

GizmosSystem* GizmosSystem_create();
void GizmosSystem_destroy(GizmosSystem* self);

typedef struct GizmosCircle
{
    float radius;
} GizmosCircle;


typedef struct GizmosRect
{
    Vec2 anchor;
    bool filled;
    float width;
    float height;
} GizmosRect;

typedef struct GizmosCmd
{
    Transform* transform;
    RenderBlendMod* blendMod;
    SDL_Color* color;
    GizmosCircle* circle;
    GizmosRect* rect;
} GizmosCmd;

void GizmosSystem_addCommand(
    GizmosSystem* self,
    const GizmosCmd* cmd
);

void GizmosSystem_clear(GizmosSystem* self);
void GizmosSystem_render(
    const GizmosSystem* self,
    SDL_Renderer* renderer,
    const Camera* camera,
    const UISystem* uiSystem
);
