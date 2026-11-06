/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_grid_layout.h"
#include "engine_common.h"

UIGridLayout* UIGridLayout_create(UISystem* uiSystem, const char* objectName, int rowCount, int columnCount)
{
    UIGridLayout* self = (UIGridLayout*)calloc(1, sizeof(UIGridLayout));
    ASSERT_NEW(self);

    UIGridLayout_init(self, uiSystem, objectName, rowCount, columnCount);

    return self;
}

void UIGridLayout_init(void* selfPtr, UISystem* uiSystem, const char* objectName, int rowCount, int columnCount)
{
    assert(selfPtr && "self must not be NULL");

    UIObject_init(selfPtr, uiSystem, objectName);
    UIObject* selfObj = (UIObject*)selfPtr;
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    selfObj->type |= UI_TYPE_GRID_LAYOUT;
    selfObj->gizmosColor = g_colors.orange5;
    selfObj->gizmosIsFilled = false;
    selfObj->gizmosOpacity = 1.f;
    selfGrid->rowCount = rowCount;
    selfGrid->colCount = columnCount;
    selfGrid->elementCount = 0;

    selfGrid->padding = Vec2_set(0.f, 0.f);
    selfGrid->anchor = Vec2_set(0.5f, 0.5f);
    selfGrid->rowSizes = (float*)calloc(rowCount, sizeof(float));
    selfGrid->colSizes = (float*)calloc(columnCount, sizeof(float));
    selfGrid->rowSpacings = (float*)calloc(rowCount, sizeof(float));
    selfGrid->colSpacings = (float*)calloc(columnCount, sizeof(float));
    selfGrid->rowOffsets = (float*)calloc((size_t)rowCount + 1, sizeof(float));
    selfGrid->colOffsets = (float*)calloc((size_t)columnCount + 1, sizeof(float));
    ASSERT_NEW(selfGrid->rowSizes);
    ASSERT_NEW(selfGrid->colSizes);
    ASSERT_NEW(selfGrid->rowSpacings);
    ASSERT_NEW(selfGrid->colSpacings);
    ASSERT_NEW(selfGrid->rowOffsets);
    ASSERT_NEW(selfGrid->colOffsets);

    for (int i = 0; i < rowCount; i++)
    {
        selfGrid->rowSizes[i] = -1.f;
    }
    for (int i = 0; i < columnCount; i++)
    {
        selfGrid->colSizes[i] = -1.f;
    }

    // Virtual methods
    selfObj->onDestroy = UIGridLayoutVM_onDestroy;
    selfObj->onRender = UIGridLayoutVM_onRender;
    selfObj->onUpdate = UIGridLayoutVM_onUpdate;
    selfObj->onDrawGizmos = UIGridLayoutVM_onDrawGizmos;
}

void UIGridLayoutVM_onDestroy(void* selfPtr)
{
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    free(selfGrid->colSizes);
    free(selfGrid->rowSizes);
    free(selfGrid->colOffsets);
    free(selfGrid->rowOffsets);
    free(selfGrid->rowSpacings);
    free(selfGrid->colSpacings);

    UIObjectVM_onDestroy(selfPtr);
}

static int UIGridLayout_findCell(void* selfPtr, void* object)
{
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    for (int i = 0; i < selfGrid->elementCount; i++)
    {
        if (selfGrid->cells[i].uiObject != object) continue;
        return i;
    }
    return -1;
}

void UIGridLayout_addObject(void* selfPtr, void* object, int rowIdx, int columnIdx, int rowSpan, int columnSpan)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    assert(UIObject_isOfType(object, UI_TYPE_OBJECT) && "object must be of type UI_TYPE_OBJECT");

    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    if (rowIdx < 0 || rowIdx >= selfGrid->rowCount ||
        columnIdx < 0 || columnIdx >= selfGrid->colCount ||
        (object == NULL))
    {
        assert(false);
        return;
    }

    rowSpan = Int_clamp(rowSpan, 1, selfGrid->rowCount - rowIdx);
    columnSpan = Int_clamp(columnSpan, 1, selfGrid->colCount - columnIdx);

    int cellIndex = UIGridLayout_findCell(selfPtr, object);
    if (cellIndex < 0)
    {
        cellIndex = selfGrid->elementCount;
        if (cellIndex >= UI_GRID_MAX_ELEMENTS)
        {
            assert(false);
            return;
        }
        selfGrid->elementCount++;
    }
    UIGridCell* cell = selfGrid->cells + cellIndex;
    cell->uiObject = object;
    cell->rowIdx = rowIdx;
    cell->colIdx = columnIdx;
    cell->rowSpan = rowSpan;
    cell->colSpan = columnSpan;

    UIObject_setParent(object, selfPtr);

    UIGridLayoutVM_onUpdate(selfPtr);
}

static void UIGridLayout_updateOffsets(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    const Vec2 dimensions = UITransform_getSize(&(selfObj->transform));
    const size_t rowCount = selfGrid->rowCount;
    const size_t colCount = selfGrid->colCount;
    const float* rowSpacings = selfGrid->rowSpacings;
    const float* colSpacings = selfGrid->colSpacings;
    const float* rowSizes = selfGrid->rowSizes;
    const float* colSizes = selfGrid->colSizes;
    float* rowOffsets = selfGrid->rowOffsets;
    float* colOffsets = selfGrid->colOffsets;
    Vec2 spacing = Vec2_zero;

    for (int i = 0; i < colCount; i++) spacing.x += colSpacings[i];
    for (int i = 0; i < rowCount; i++) spacing.y += rowSpacings[i];

    Vec2 fixedDim = Vec2_add(Vec2_scale(selfGrid->padding, 2.f), spacing);
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

void UIGridLayoutVM_onUpdate(void* selfPtr)
{
    UIObjectVM_onUpdate(selfPtr);
    UIGridLayout_updateOffsets(selfPtr);

    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    UIObject* selfObj = (UIObject*)selfPtr;
    const size_t rowCount = selfGrid->rowCount;
    const size_t colCount = selfGrid->colCount;
    const float* rowOffsets = selfGrid->rowOffsets;
    const float* colOffsets = selfGrid->colOffsets;
    const float* rowSpacings = selfGrid->rowSpacings;
    const float* colSpacings = selfGrid->colSpacings;
    const Vec2 padding = selfGrid->padding;
    const Vec2 gridDim = Vec2_set(
        colOffsets[selfGrid->colCount],
        rowOffsets[selfGrid->rowCount]
    );
    Vec2 availableDim = UITransform_getSize(&(selfObj->transform));
    availableDim.x -= 2.f * padding.x;
    availableDim.y -= 2.f * padding.y;
    const Vec2 gridAnchor = selfGrid->anchor;
    const Vec2 gridOffset = Vec2_set(
        +padding.x + gridAnchor.x * (availableDim.x - gridDim.x),
        +padding.y + (gridAnchor.y - 1.f) * (availableDim.y - gridDim.y) + availableDim.y
    );

    for (int i = 0; i < selfGrid->elementCount; i++)
    {
        UIGridCell cell = selfGrid->cells[i];
        UIObject* object = (UIObject*)(cell.uiObject);
        UIRect* rect = &(object->transform.localRect);

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

void UIGridLayoutVM_onDrawGizmos(void* selfPtr, GizmosSystem* gizmosSystem)
{
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    UIObject* selfObj = (UIObject*)selfPtr;
    UISystem* uiSystem = selfObj->uiSystem;
    const size_t rowCount = selfGrid->rowCount;
    const size_t colCount = selfGrid->colCount;
    const float* rowOffsets = selfGrid->rowOffsets;
    const float* colOffsets = selfGrid->colOffsets;
    const float* rowSpacings = selfGrid->rowSpacings;
    const float* colSpacings = selfGrid->colSpacings;
    const Vec2 padding = selfGrid->padding;
    const Vec2 gridDim = Vec2_set(
        colOffsets[selfGrid->colCount],
        rowOffsets[selfGrid->rowCount]
    );
    Vec2 availableDim = UITransform_getSize(&(selfObj->transform));
    availableDim.x -= 2.f * padding.x;
    availableDim.y -= 2.f * padding.y;
    const Vec2 gridAanchor = selfGrid->anchor;
    const Vec2 gridOffset = Vec2_set(
        +padding.x + gridAanchor.x * (availableDim.x - gridDim.x),
        +padding.y + (gridAanchor.y - 1.f) * (availableDim.y - gridDim.y) + availableDim.y
    );
    const AABB* aabb = &(selfObj->transform.aabb);

    Transform transform = { 0 };
    GizmosRect gizmosRect = { 0 };
    RenderBlendMod blendMod = { 0 };

    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.rect = &gizmosRect;
    cmd.color = &selfObj->gizmosColor;
    cmd.blendMod = &blendMod;

    gizmosRect.anchor = Vec2_anchor_south_west;
    gizmosRect.filled = selfObj->gizmosIsFilled;
    blendMod.mode = SDL_BLENDMODE_BLEND;
    blendMod.alpha = selfObj->gizmosOpacity;
    transform.space = TRANSFORM_SPACE_UI;

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

            gizmosRect.width = childAABB.upper.x - childAABB.lower.x;
            gizmosRect.height = childAABB.upper.y - childAABB.lower.y;
            transform.position = childAABB.lower;

            GizmosSystem_addCommand(gizmosSystem, &cmd);
        }
    }
}

void UIGridLayout_setRowSize(void* selfPtr, int index, float size)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    assert(index >= 0 && index < selfGrid->rowCount && "index out of bounds");
    selfGrid->rowSizes[index] = size;
}

void UIGridLayout_setColumnSize(void* selfPtr, int index, float size)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    assert(index >= 0 && index < selfGrid->colCount && "index out of bounds");
    selfGrid->colSizes[index] = size;
}

void UIGridLayout_setRowSizes(void* selfPtr, float size)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    for (int i = 0; i < selfGrid->rowCount; i++)
    {
        selfGrid->rowSizes[i] = size;
    }
}

void UIGridLayout_setColumnSizes(void* selfPtr, float size)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    for (int i = 0; i < selfGrid->colCount; i++)
    {
        selfGrid->colSizes[i] = size;
    }
}

void UIGridLayout_setRowSpacing(void* selfPtr, int index, float spacing)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    assert(index >= 0 && index < selfGrid->rowCount - 1 && "index out of bounds");
    selfGrid->rowSpacings[index] = spacing;
}

void UIGridLayout_setColumnSpacing(void* selfPtr, int index, float spacing)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    assert(index >= 0 && index < selfGrid->colCount - 1 && "index out of bounds");
    selfGrid->colSpacings[index] = spacing;
}

void UIGridLayout_setRowSpacings(void* selfPtr, float spacing)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    for (int i = 0; i < selfGrid->rowCount - 1; i++)
    {
        selfGrid->rowSpacings[i] = spacing;
    }
    selfGrid->rowSpacings[selfGrid->rowCount - 1] = 0.f;
}

void UIGridLayout_setColumnSpacings(void* selfPtr, float spacing)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    for (int i = 0; i < selfGrid->colCount - 1; i++)
    {
        selfGrid->colSpacings[i] = spacing;
    }
    selfGrid->colSpacings[selfGrid->colCount - 1] = 0.f;
}

void UIGridLayout_setPadding(void* selfPtr, Vec2 padding)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    selfGrid->padding = padding;
}

void UIGridLayout_setAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_GRID_LAYOUT) && "self must be of type UI_TYPE_GRID_LAYOUT");
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    selfGrid->anchor = anchor;
}

Vec2 UIGridLayout_getMinimumSize(void* selfPtr)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UIGridLayout* selfGrid = (UIGridLayout*)selfPtr;
    const size_t rowCount = selfGrid->rowCount;
    const size_t colCount = selfGrid->colCount;
    const float* rowSpacings = selfGrid->rowSpacings;
    const float* colSpacings = selfGrid->colSpacings;
    const float* rowSizes = selfGrid->rowSizes;
    const float* colSizes = selfGrid->colSizes;
    Vec2 spacing = Vec2_zero;

    for (int i = 0; i < colCount; i++) spacing.x += colSpacings[i];
    for (int i = 0; i < rowCount; i++) spacing.y += rowSpacings[i];

    Vec2 fixedDim = Vec2_add(Vec2_scale(selfGrid->padding, 2.f), spacing);
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
