/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_selectable.h"
#include "ui/ui_canvas.h"

#define MAX_UI_SELECTABLES_IN_GROUP 256

typedef struct UIFocusManagerElement
{
    UISelectable* m_selectable;
    int m_id;
} UIFocusManagerElement;

typedef struct UIFocusManager
{
    UICanvas* m_canvas;

    UIFocusManagerElement m_elements[MAX_UI_SELECTABLES_IN_GROUP];
    int m_elementCount;

    UISelectable* m_focused;
    bool m_enabled;
} UIFocusManager;

UIFocusManager* UIFocusManager_create();
void UIFocusManager_destroy(UIFocusManager* self);
void UIFocusManager_update(UIFocusManager* self, UIInput* input);

void UIFocusManager_setCanvas(UIFocusManager* self, void* canvas);
void UIFocusManager_addSelectable(UIFocusManager* self, void* selectable);
void UIFocusManager_removeSelectable(UIFocusManager* self, void* selectable);
void UIFocusManager_clear(UIFocusManager* self);
void UIFocusManager_setFocused(UIFocusManager* self, void* selectable);

// TODO: Définir des groupes de navigation pour gérer plusieurs groupes indépendants UIFocusGroup.
// Un groupe aurait un selectable de référence par direction (up, down, left, right) pour gérer la navigation entre groupes.
// Au sein d'un groupe, la navigation se fait comme actuellement.
