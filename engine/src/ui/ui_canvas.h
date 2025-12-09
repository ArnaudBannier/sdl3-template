/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "math.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"

void UICanvasVM_onRender(void* self);
void UICanvasVM_onUpdate(void* self);
#define UICanvasVM_onDestroy UIObjectVM_onDestroy

#define UI_CANVAS_MAX_ELEMENTS 128
#define UI_CANVAS_SET_CAPACITY 256

typedef struct UICanvas
{
    UIObject m_baseObject;
    int m_set[UI_CANVAS_SET_CAPACITY];
    int m_setSize;
} UICanvas;

UICanvas* UICanvas_create(const char* objectName);
void UICanvas_init(void* self, const char* objectName);
INLINE void UICanvas_destroy(void* self)
{
    UIObject_destroy(self);
}

void UICanvas_recordObjects(UICanvas* self);
bool UICanvas_hasObject(void* self, int id);
