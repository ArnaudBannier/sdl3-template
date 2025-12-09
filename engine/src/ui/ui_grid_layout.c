/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_grid_layout.h"
#include "game_engine_common.h"

UIGridLayout* UIGridLayout_create(const char* objectName, int rowCount, int columnCount)
{
    UIGridLayout* self = (UIGridLayout*)calloc(1, sizeof(UIGridLayout));
    AssertNew(self);

    UIGridLayout_init(self, objectName, rowCount, columnCount);

    return self;
}

void UIGridLayout_init(void* self, const char* objectName, int rowCount, int columnCount)
{
    assert(self && "self must not be NULL");

    UIObject_init(self, objectName);
    UIObject* selfObj = (UIObject*)self;
    UIGridLayout* selfGrid = (UIGridLayout*)self;

    selfObj->m_type |= UI_TYPE_GRID_LAYOUT;
    selfGrid->m_rowCount = rowCount;
    selfGrid->m_colCount = columnCount;
    selfGrid->m_elementCount = 0;

    selfGrid->m_padding = Vec2_set(0.f, 0.f);
    selfGrid->m_anchor = Vec2_set(0.5f, 0.5f);
    selfGrid->m_rowSizes = (float*)calloc(rowCount, sizeof(float));
    selfGrid->m_colSizes = (float*)calloc(columnCount, sizeof(float));
    selfGrid->m_rowSpacings = (float*)calloc(rowCount, sizeof(float));
    selfGrid->m_colSpacings = (float*)calloc(columnCount, sizeof(float));
    selfGrid->m_rowOffsets = (float*)calloc((size_t)rowCount + 1, sizeof(float));
    selfGrid->m_colOffsets = (float*)calloc((size_t)columnCount + 1, sizeof(float));
    AssertNew(selfGrid->m_rowSizes);
    AssertNew(selfGrid->m_colSizes);
    AssertNew(selfGrid->m_rowSpacings);
    AssertNew(selfGrid->m_colSpacings);
    AssertNew(selfGrid->m_rowOffsets);
    AssertNew(selfGrid->m_colOffsets);

    for (int i = 0; i < rowCount; i++)
    {
        selfGrid->m_rowSizes[i] = -1.f;
    }
    for (int i = 0; i < columnCount; i++)
    {
        selfGrid->m_colSizes[i] = -1.f;
    }

    // Virtual methods
    selfObj->m_onDestroy = UIGridLayoutVM_onDestroy;
    selfObj->m_onRender = UIGridLayoutVM_onRender;
    selfObj->m_onUpdate = UIGridLayoutVM_onUpdate;
}

void UIGridLayoutVM_onDestroy(void* self)
{
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    free(selfGrid->m_colSizes);
    free(selfGrid->m_rowSizes);
    free(selfGrid->m_colOffsets);
    free(selfGrid->m_rowOffsets);
    free(selfGrid->m_rowSpacings);
    free(selfGrid->m_colSpacings);

    UIObjectVM_onDestroy(self);
}

static int UIGridLayout_findCell(void* self, void* object)
{
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    for (int i = 0; i < selfGrid->m_elementCount; i++)
    {
        if (selfGrid->m_cells[i].uiObject != object) continue;
        return i;
    }
    return -1;
}

void UIGridLayout_addObject(void* self, void* object, int rowIdx, int columnIdx, int rowSpan, int columnSpan)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    assert(UIObject_isOfType(object, UI_TYPE_OBJECT) && "uiObject must be of type UI_TYPE_OBJECT");

    UIGridLayout* selfGrid = (UIGridLayout*)self;
    if (rowIdx < 0 || rowIdx >= selfGrid->m_rowCount ||
        columnIdx < 0 || columnIdx >= selfGrid->m_colCount ||
        (object == NULL))
    {
        assert(false);
        return;
    }

    rowSpan = Int_clamp(rowSpan, 1, selfGrid->m_rowCount - rowIdx);
    columnSpan = Int_clamp(columnSpan, 1, selfGrid->m_colCount - columnIdx);

    int cellIndex = UIGridLayout_findCell(self, object);
    if (cellIndex < 0)
    {
        cellIndex = selfGrid->m_elementCount;
        if (cellIndex >= UI_GRID_MAX_ELEMENTS)
        {
            assert(false);
            return;
        }
        selfGrid->m_elementCount++;
    }
    UIGridCell* cell = selfGrid->m_cells + cellIndex;
    cell->uiObject = object;
    cell->rowIdx = rowIdx;
    cell->colIdx = columnIdx;
    cell->rowSpan = rowSpan;
    cell->colSpan = columnSpan;

    UIObject_setParent(object, self);

    UIGridLayoutVM_onUpdate(self);
}

static void UIGridLayout_updateOffsets(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    const Vec2 dimensions = UITransform_getSize(&(selfObj->m_transform));
    const size_t rowCount = selfGrid->m_rowCount;
    const size_t colCount = selfGrid->m_colCount;
    const float* rowSpacings = selfGrid->m_rowSpacings;
    const float* colSpacings = selfGrid->m_colSpacings;
    const float* rowSizes = selfGrid->m_rowSizes;
    const float* colSizes = selfGrid->m_colSizes;
    float* rowOffsets = selfGrid->m_rowOffsets;
    float* colOffsets = selfGrid->m_colOffsets;
    Vec2 spacing = Vec2_zero;

    for (int i = 0; i < colCount; i++) spacing.x += colSpacings[i];
    for (int i = 0; i < rowCount; i++) spacing.y += rowSpacings[i];

    Vec2 fixedDim = Vec2_add(Vec2_scale(selfGrid->m_padding, 2.f), spacing);
    Vec2 stretchSum = Vec2_zero;
    for (int i = 0; i < colCount; i++)
    {
        float size = colSizes[i];
        if (size > 0.f) fixedDim.x += size;
        else stretchSum.x += size;
    }
    for (int i = 0; i < rowCount; i++)
    {
        float size = rowSizes[i];
        if (size > 0.f) fixedDim.y += size;
        else stretchSum.y += size;
    }

    Vec2 stretchSize = Vec2_sub(dimensions, fixedDim);
    if (stretchSum.x < 0.f) stretchSize.x /= stretchSum.x;
    if (stretchSum.y < 0.f) stretchSize.y /= stretchSum.y;

    rowOffsets[0] = 0.f;
    for (size_t i = 0; i < rowCount; i++)
    {
        rowOffsets[i + 1] = rowOffsets[i];
        rowOffsets[i + 1] += rowSpacings[i];

        if (rowSizes[i] > 0.f)
        {
            rowOffsets[i + 1] += rowSizes[i];
        }
        else
        {
            rowOffsets[i + 1] += rowSizes[i] * stretchSize.y;
        }
    }

    colOffsets[0] = 0.f;
    for (size_t i = 0; i < colCount; i++)
    {
        colOffsets[i + 1] = colOffsets[i];
        colOffsets[i + 1] += colSpacings[i];
        if (colSizes[i] > 0.f)
        {
            colOffsets[i + 1] += colSizes[i];
        }
        else
        {
            colOffsets[i + 1] += colSizes[i] * stretchSize.x;
        }
    }
}

void UIGridLayoutVM_onUpdate(void* self)
{
    UIObjectVM_onUpdate(self);
    UIGridLayout_updateOffsets(self);

    UIGridLayout* selfGrid = (UIGridLayout*)self;
    UIObject* selfObj = (UIObject*)self;
    const size_t rowCount = selfGrid->m_rowCount;
    const size_t colCount = selfGrid->m_colCount;
    const float* rowOffsets = selfGrid->m_rowOffsets;
    const float* colOffsets = selfGrid->m_colOffsets;
    const float* rowSpacings = selfGrid->m_rowSpacings;
    const float* colSpacings = selfGrid->m_colSpacings;
    const Vec2 padding = selfGrid->m_padding;
    const Vec2 gridDim = Vec2_set(
        colOffsets[selfGrid->m_colCount],
        rowOffsets[selfGrid->m_rowCount]
    );
    Vec2 availableDim = UITransform_getSize(&(selfObj->m_transform));
    availableDim.x -= 2.f * padding.x;
    availableDim.y -= 2.f * padding.y;
    const Vec2 anchor = selfGrid->m_anchor;
    const Vec2 gridOffset = Vec2_set(
        +padding.x + anchor.x * (availableDim.x - gridDim.x),
        +padding.y + (anchor.y - 1.f) * (availableDim.y - gridDim.y) + availableDim.y
    );

    for (int i = 0; i < selfGrid->m_elementCount; i++)
    {
        UIGridCell cell = selfGrid->m_cells[i];
        UIObject* object = (UIObject*)(cell.uiObject);
        UIRect* rect = &(object->m_transform.localRect);

        rect->anchorMin = Vec2_zero;
        rect->anchorMax = Vec2_zero;
        rect->offsetMin = gridOffset;
        rect->offsetMax = gridOffset;

        rect->offsetMin.x += colOffsets[cell.colIdx];
        rect->offsetMax.x += colOffsets[cell.colIdx + cell.colSpan] - colSpacings[cell.colIdx + cell.colSpan - 1];
        rect->offsetMin.y -= rowOffsets[cell.rowIdx + cell.rowSpan] - rowSpacings[cell.rowIdx + cell.rowSpan - 1];
        rect->offsetMax.y -= rowOffsets[cell.rowIdx];
    }
}

void UIGridLayoutVM_onRender(void* self)
{
    if (!g_drawUIGizmos) return;

    UIGridLayout* selfGrid = (UIGridLayout*)self;
    UIObject* selfObj = (UIObject*)self;
    const size_t rowCount = selfGrid->m_rowCount;
    const size_t colCount = selfGrid->m_colCount;
    const float* rowOffsets = selfGrid->m_rowOffsets;
    const float* colOffsets = selfGrid->m_colOffsets;
    const float* rowSpacings = selfGrid->m_rowSpacings;
    const float* colSpacings = selfGrid->m_colSpacings;
    const Vec2 padding = selfGrid->m_padding;
    const Vec2 gridDim = Vec2_set(
        colOffsets[selfGrid->m_colCount],
        rowOffsets[selfGrid->m_rowCount]
    );
    Vec2 availableDim = UITransform_getSize(&(selfObj->m_transform));
    availableDim.x -= 2.f * padding.x;
    availableDim.y -= 2.f * padding.y;
    const Vec2 anchor = selfGrid->m_anchor;
    const Vec2 gridOffset = Vec2_set(
        +padding.x + anchor.x * (availableDim.x - gridDim.x),
        -padding.y + (anchor.y - 1.f) * (availableDim.y - gridDim.y) + availableDim.y
    );
    const AABB* aabb = &(selfObj->m_transform.aabb);

    SDL_Color color = g_colors.grape5;
    SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, 255);

    AABB childAABB = { 0 };
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
        {
            childAABB.lower = Vec2_add(gridOffset, aabb->lower);
            childAABB.upper = childAABB.lower;

            childAABB.lower.x += colOffsets[j];
            childAABB.upper.x += colOffsets[j + 1] - colSpacings[j];
            childAABB.lower.y -= rowOffsets[i + 1] - rowSpacings[i];
            childAABB.upper.y -= rowOffsets[i];

            SDL_FRect rect = UIRect_aabbToViewportRect(&childAABB);
            SDL_RenderRect(g_renderer, &rect);
        }
    }
}

void UIGridLayout_setRowSize(void* self, int index, float size)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    assert(index >= 0 && index < selfGrid->m_rowCount && "index out of bounds");
    selfGrid->m_rowSizes[index] = size;
}

void UIGridLayout_setColumnSize(void* self, int index, float size)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    assert(index >= 0 && index < selfGrid->m_colCount && "index out of bounds");
    selfGrid->m_colSizes[index] = size;
}

void UIGridLayout_setRowSizes(void* self, float size)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    for (int i = 0; i < selfGrid->m_rowCount; i++)
    {
        selfGrid->m_rowSizes[i] = size;
    }
}

void UIGridLayout_setColumnSizes(void* self, float size)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    for (int i = 0; i < selfGrid->m_colCount; i++)
    {
        selfGrid->m_colSizes[i] = size;
    }
}

void UIGridLayout_setRowSpacing(void* self, int index, float spacing)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    assert(index >= 0 && index < selfGrid->m_rowCount - 1 && "index out of bounds");
    selfGrid->m_rowSpacings[index] = spacing;
}

void UIGridLayout_setColumnSpacing(void* self, int index, float spacing)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    assert(index >= 0 && index < selfGrid->m_colCount - 1 && "index out of bounds");
    selfGrid->m_colSpacings[index] = spacing;
}

void UIGridLayout_setRowSpacings(void* self, float spacing)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    for (int i = 0; i < selfGrid->m_rowCount - 1; i++)
    {
        selfGrid->m_rowSpacings[i] = spacing;
    }
    selfGrid->m_rowSpacings[selfGrid->m_rowCount - 1] = 0.f;
}

void UIGridLayout_setColumnSpacings(void* self, float spacing)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    for (int i = 0; i < selfGrid->m_colCount - 1; i++)
    {
        selfGrid->m_colSpacings[i] = spacing;
    }
    selfGrid->m_colSpacings[selfGrid->m_colCount - 1] = 0.f;
}

void UIGridLayout_setPadding(void* self, Vec2 padding)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    selfGrid->m_padding = padding;
}

void UIGridLayout_setAnchor(void* self, Vec2 anchor)
{
    assert(UIObject_isOfType(self, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    selfGrid->m_anchor = anchor;
}

Vec2 UIGridLayout_getMinimumSize(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UIGridLayout* selfGrid = (UIGridLayout*)self;
    const size_t rowCount = selfGrid->m_rowCount;
    const size_t colCount = selfGrid->m_colCount;
    const float* rowSpacings = selfGrid->m_rowSpacings;
    const float* colSpacings = selfGrid->m_colSpacings;
    const float* rowSizes = selfGrid->m_rowSizes;
    const float* colSizes = selfGrid->m_colSizes;
    Vec2 spacing = Vec2_zero;

    for (int i = 0; i < colCount; i++) spacing.x += colSpacings[i];
    for (int i = 0; i < rowCount; i++) spacing.y += rowSpacings[i];

    Vec2 fixedDim = Vec2_add(Vec2_scale(selfGrid->m_padding, 2.f), spacing);
    Vec2 stretchSum = Vec2_zero;
    for (int i = 0; i < colCount; i++)
    {
        float size = colSizes[i];
        if (size > 0.f) fixedDim.x += size;
    }
    for (int i = 0; i < rowCount; i++)
    {
        float size = rowSizes[i];
        if (size > 0.f) fixedDim.y += size;
    }

    return fixedDim;
}
