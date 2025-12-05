/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"

void UIGridLayoutVM_onRender(void* self);
void UIGridLayoutVM_onUpdate(void* self);
void UIGridLayoutVM_onDestroy(void* self);

typedef struct UIGridCell
{
    void* uiObject;
    size_t rowIdx;
    size_t colIdx;
    size_t rowSpan;
    size_t colSpan;
} UIGridCell;

#define UI_GRID_MAX_ELEMENTS 64

typedef struct UIGridLayout
{
    UIObject m_baseObject;

    int m_rowCount;
    int m_colCount;
    int m_elementCount;
    Vec2 m_padding;
    Vec2 m_anchor;
    float* m_rowSizes;
    float* m_colSizes;
    float* m_rowSpacings;
    float* m_colSpacings;
    float* m_rowOffsets;
    float* m_colOffsets;
    UIGridCell m_cells[UI_GRID_MAX_ELEMENTS];
} UIGridLayout;

UIGridLayout* UIGridLayout_create(const char* objectName, int rowCount, int columnCount);
void UIGridLayout_init(void* self, const char* objectName, int rowCount, int columnCount);
INLINE void UIGridLayout_destroy(void* self)
{
    UIObject_destroy(self);
}

void UIGridLayout_addObject(void* self, void* object, int rowIdx, int columnIdx, int rowSpan, int columnSpan);

void UIGridLayout_setRowSize(void* self, int index, float size);
void UIGridLayout_setColumnSize(void* self, int index, float size);
void UIGridLayout_setRowSizes(void* self, float size);
void UIGridLayout_setColumnSizes(void* self, float size);

void UIGridLayout_setRowSpacing(void* self, int index, float spacing);
void UIGridLayout_setColumnSpacing(void* self, int index, float spacing);
void UIGridLayout_setRowSpacings(void* self, float spacing);
void UIGridLayout_setColumnSpacings(void* self, float spacing);

void UIGridLayout_setPadding(void* self, Vec2 padding);
void UIGridLayout_setAnchor(void* self, Vec2 anchor);
Vec2 UIGridLayout_getMinimumSize(void* self);
