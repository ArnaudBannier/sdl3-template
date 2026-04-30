/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"
#include "core/camera.h"
#include "core/components.h"
#include "core/anim/sprite_anim.h"
#include "core/anim/sprite_anim_manager.h"

typedef struct UISystem UISystem;
typedef struct GraphicsCoreCmd GraphicsCoreCmd;

typedef struct GraphicsSystem
{
    GraphicsCoreCmd* m_commands;
    size_t m_count;
    size_t m_capacity;
    float m_renderScale;
} GraphicsSystem;

GraphicsSystem* GraphicsSystem_create();

void GraphicsSystem_destroy(GraphicsSystem* self);

typedef struct GraphicsCmd
{
    RenderTexture* texture;
    RenderRect* rect;
    RenderText* text;

    RenderSortingLayer* sortingLayer;
    Transform* transform;
    RenderDim* dimensions;
    RenderAnchor* anchor;

    RenderBlendMod* blendMod;
    RenderColorMod* colorMod;
    RenderFlip* flip;
} GraphicsCmd;

void GraphicsSystem_addCommand(
    GraphicsSystem* self,
    const GraphicsCmd* cmd
);

INLINE void GraphicsSystem_setRenderScale(GraphicsSystem* self, float renderScale)
{
    assert(self && "self must not be NULL");
    self->m_renderScale = renderScale;
}

void GraphicsSystem_clear(GraphicsSystem* self);

void GraphicsSystem_render(
    const GraphicsSystem* self,
    SDL_Renderer* renderer,
    const Camera* camera,
    const UISystem* uiSystem
);
