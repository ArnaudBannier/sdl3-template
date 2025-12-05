/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/game_graphics.h"
#include "common/assets.h"
#include "common/common.h"
#include "game/input.h"
#include "game/scene.h"
#include "game/game_config.h"

GameGraphics* GameGraphics_create(Scene* scene)
{
    GameGraphics* self = (GameGraphics*)calloc(1, sizeof(GameGraphics));
    AssertNew(self);

    self->m_scene = scene;
    self->m_padding = Vec2_set(0.0f, 0.0f);
    self->m_spacing = Vec2_set(0.1f, 0.1f);
    self->m_gridAABB.lower = Vec2_add(Vec2_set(-4.f, -4.f), Vec2_set(8.0f, 4.5f));
    self->m_gridAABB.upper = Vec2_add(Vec2_set(+4.f, +4.f), Vec2_set(8.0f, 4.5f));
    self->m_enabled = false;

    AssetManager* assets = Scene_getAssetManager(scene);
    SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_GAME);
    AssertNew(spriteSheet);
    self->m_spriteRabbit = SpriteSheet_getGroupByName(spriteSheet, "rabbit");
    AssertNew(self->m_spriteRabbit);

    return self;
}

void GameGraphics_destroy(GameGraphics* self)
{
    if (!self) return;
    free(self);
}

static void GameGraphics_updateCells(GameGraphics* self)
{
    float totalPaddingX = self->m_padding.x * 2.f;
    float totalPaddingY = self->m_padding.y * 2.f;
    float totalSpacingX = self->m_spacing.x * (GAME_GRID_SIZE - 1);
    float totalSpacingY = self->m_spacing.y * (GAME_GRID_SIZE - 1);
    Vec2 gridSize = AABB_getSize(&(self->m_gridAABB));
    float cellW = (gridSize.x - totalPaddingX - totalSpacingX) / GAME_GRID_SIZE;
    float cellH = (gridSize.y - totalPaddingY - totalSpacingY) / GAME_GRID_SIZE;

    for (int i = 0; i < GAME_GRID_SIZE; i++)
    {
        for (int j = 0; j < GAME_GRID_SIZE; j++)
        {
            float cellX = self->m_gridAABB.lower.x + self->m_padding.x + j * (cellW + self->m_spacing.x);
            float cellY = self->m_gridAABB.lower.y + self->m_padding.y + i * (cellH + self->m_spacing.y);
            self->m_cells[GAME_GRID_SIZE - 1 - i][j].lower = Vec2_set(cellX, cellY);
            self->m_cells[GAME_GRID_SIZE - 1 - i][j].upper = Vec2_set(cellX + cellW, cellY + cellH);
        }
    }
}

void GameGraphics_update(GameGraphics* self)
{
    assert(self && "self must not be NULL");
    Scene* scene = self->m_scene;
    Input* input = Scene_getInput(scene);
    Camera* camera = Scene_getCamera(scene);

    GameGraphics_updateCells(self);

    Vec2 mouseWorldPos = { 0 };
    Camera_viewToWorld(camera, input->mouse.position.x, input->mouse.position.y, &mouseWorldPos);

    if (self->m_enabled == false)
    {
        return;
    }

    if (input->mouse.leftPressed)
    {
        for (int i = 0; i < GAME_GRID_SIZE; i++)
        {
            for (int j = 0; j < GAME_GRID_SIZE; j++)
            {
                AABB* cellAABB = &(self->m_cells[i][j]);
                if (AABB_containsPoint(cellAABB, mouseWorldPos))
                {
                    self->m_selectedRowIndex = i;
                    self->m_selectedColIndex = j;
                }
            }
        }
    }
}

void GameGraphics_render(GameGraphics* self)
{
    assert(self && "self must not be NULL");

    Scene* scene = self->m_scene;
    Camera* camera = Scene_getCamera(scene);
    float scale = Camera_getWorldToViewScale(camera);

    SDL_FRect rect = { 0 };
    for (int i = 0; i < GAME_GRID_SIZE; i++)
    {
        for (int j = 0; j < GAME_GRID_SIZE; j++)
        {
            AABB* cellAABB = &(self->m_cells[i][j]);
            rect.x = Camera_worldToViewX(camera, cellAABB->lower.x);
            rect.y = Camera_worldToViewY(camera, cellAABB->upper.y);
            rect.w = (cellAABB->upper.x - cellAABB->lower.x) * scale;
            rect.h = (cellAABB->upper.y - cellAABB->lower.y) * scale;

            bool isSelected = (i == self->m_selectedRowIndex && j == self->m_selectedColIndex);

            SDL_Color color = isSelected ? g_colors.orange9 : g_colors.gray8;
            SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(g_renderer, &rect);

            if (isSelected)
            {
                SpriteGroup_render(self->m_spriteRabbit, 0, &rect, Vec2_anchor_north_west, 1.0f);
            }
        }
    }
}
