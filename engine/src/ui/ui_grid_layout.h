/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "utils/utils.h"
#include "core/sprite_sheet.h"

void UIGridLayoutVM_onUpdate(void* selfPtr);
void UIGridLayoutVM_onDestroy(void* selfPtr);
void UIGridLayoutVM_onDrawGizmos(void* selfPtr, GizmosSystem* gizmosSystem);
#define UIGridLayoutVM_onRender UIObjectVM_onRender

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

UIGridLayout* UIGridLayout_create(UISystem* uiSystem, const char* objectName, int rowCount, int columnCount);
void UIGridLayout_init(void* selfPtr, UISystem* uiSystem, const char* objectName, int rowCount, int columnCount);
INLINE void UIGridLayout_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

void UIGridLayout_addObject(void* selfPtr, void* object, int rowIdx, int columnIdx, int rowSpan, int columnSpan);

void UIGridLayout_setRowSize(void* selfPtr, int index, float size);
void UIGridLayout_setColumnSize(void* selfPtr, int index, float size);
void UIGridLayout_setRowSizes(void* selfPtr, float size);
void UIGridLayout_setColumnSizes(void* selfPtr, float size);

void UIGridLayout_setRowSpacing(void* selfPtr, int index, float spacing);
void UIGridLayout_setColumnSpacing(void* selfPtr, int index, float spacing);
void UIGridLayout_setRowSpacings(void* selfPtr, float spacing);
void UIGridLayout_setColumnSpacings(void* selfPtr, float spacing);

void UIGridLayout_setPadding(void* selfPtr, Vec2 padding);
void UIGridLayout_setAnchor(void* selfPtr, Vec2 anchor);
Vec2 UIGridLayout_getMinimumSize(void* selfPtr);
