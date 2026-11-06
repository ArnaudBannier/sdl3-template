/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/puzzle/puzzle_graphics.h"
#include "game/puzzle/puzzle_core.h"

PuzzleGraphics* PuzzleGraphics_create(GameContext* context)
{
    PuzzleGraphics* self = (PuzzleGraphics*)calloc(1, sizeof(PuzzleGraphics));
    ASSERT_NEW(self);

    self->context = context;
    self->enabled = true;
    self->selectedRow = -1;
    self->selectedCol = -1;
    self->spacing = 10.f;

    // Canvas = 640x360
    Vec2 size = Vec2_set(640.f, 360.f);
    Vec2 center = Vec2_scale(size, 0.5f);
    Vec2 puzzleDim = Vec2_set(450.f, 300.f);
    self->puzzleAABB.lower = Vec2_add(center, Vec2_scale(puzzleDim, -0.5f));
    self->puzzleAABB.upper = Vec2_add(center, Vec2_scale(puzzleDim, +0.5f));

    return self;
}

void PuzzleGraphics_destroy(PuzzleGraphics* self)
{
    if (!self) return;
    free(self);
}

static void PuzzleGraphics_updateCells(PuzzleGraphics* self)
{
    float totalSpacingX = self->spacing * (PUZZLE_COL_COUNT - 1);
    float totalSpacingY = self->spacing * (PUZZLE_ROW_COUNT - 1);
    Vec2 puzzleSize = AABB_getSize(&(self->puzzleAABB));
    float cellW = (puzzleSize.x - totalSpacingX) / PUZZLE_COL_COUNT;
    float cellH = (puzzleSize.y - totalSpacingY) / PUZZLE_ROW_COUNT;

    for (int i = 0; i < PUZZLE_ROW_COUNT; i++)
    {
        for (int j = 0; j < PUZZLE_COL_COUNT; j++)
        {
            float cellX = self->puzzleAABB.lower.x + j * (cellW + self->spacing);
            float cellY = self->puzzleAABB.lower.y + i * (cellH + self->spacing);
            self->cells[PUZZLE_ROW_COUNT - 1 - i][j].lower = Vec2_set(cellX, cellY);
            self->cells[PUZZLE_ROW_COUNT - 1 - i][j].upper = Vec2_set(cellX + cellW, cellY + cellH);
        }
    }
}

void PuzzleGraphics_update(PuzzleGraphics* self)
{
    assert(self && "self must not be NULL");
    GameContext* context = self->context;

    PuzzleGraphics_updateCells(self);

    if (self->enabled == false)
    {
        return;
    }

    Input* input = GameContext_getInput(context);
    UIInput* uiInput = &(input->uiInput);
    Vec2 mousePos = uiInput->mouseUIPos;

    if (input->mouse.leftPressed)
    {
        printf("Mouse clicked at: (%.2f, %.2f)\n", mousePos.x, mousePos.y);
        for (int i = 0; i < PUZZLE_ROW_COUNT; i++)
        {
            for (int j = 0; j < PUZZLE_COL_COUNT; j++)
            {
                AABB* cellAABB = &(self->cells[i][j]);
                if (AABB_containsPoint(cellAABB, mousePos))
                {
                    self->selectedRow = i;
                    self->selectedCol = j;
                }
            }
        }
    }

}

void PuzzleGraphics_render(PuzzleGraphics* self)
{
    assert(self && "self must not be NULL");

    if (self->enabled == false)
    {
        return;
    }

    GameContext* context = self->context;
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);

    Transform transform = { 0 };
    RenderAnchor anchor = { .value = Vec2_anchor_south_west };
    RenderSortingLayer sortingLayer = { 0 };
    RenderDim dimensions = { 0 };

    transform.space = TRANSFORM_SPACE_UI;
    transform.position = Vec2_set(2.f, 2.f);
    dimensions.space = RENDER_DIM_SPACE_UI;
    dimensions.width = 1.0f;
    dimensions.height = 1.0f;
    sortingLayer.layer = DEFAULT_UI_LAYER + 5;

    RenderRect rect = { 0 };
    rect.color = g_colors.blue5;
    rect.filled = true;

    for (int i = 0; i < PUZZLE_ROW_COUNT; i++)
    {
        for (int j = 0; j < PUZZLE_COL_COUNT; j++)
        {
            AABB cellAABB = self->cells[i][j];
            dimensions.width = cellAABB.upper.x - cellAABB.lower.x;
            dimensions.height = cellAABB.upper.y - cellAABB.lower.y;
            transform.position = cellAABB.lower;

            if (i == self->selectedRow && j == self->selectedCol)
            {
                rect.color = g_colors.yellow5;
            }
            else
            {
                rect.color = g_colors.blue5;
            }

            GraphicsCmd cmd = { 0 };
            cmd.transform = &transform;
            cmd.anchor = &anchor;
            cmd.dimensions = &dimensions;
            cmd.rect = &rect;
            cmd.sortingLayer = &sortingLayer;
            GraphicsSystem_addCommand(graphicsSystem, &cmd);
        }
    }
}
