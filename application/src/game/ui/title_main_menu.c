/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/title_main_menu.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"
#include "game/core/game_engine.h"

static void TitleMainMenu_onClick(void* selectable)
{
    TitleMainMenu* self = (TitleMainMenu*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    const char* objectName = UIObject_getObjectName(selectable);
    if (strcmp(objectName, "start-button") == 0)
    {
        printf("Start button clicked\n");
    }
    else if (strcmp(objectName, "settings-button") == 0)
    {
        TitleUIManager_closePages(self->m_uiManager);
        TitleUIManager_openPage(self->m_uiManager, TITLE_UI_PAGE_SETTINGS);
    }
    else if (strcmp(objectName, "quit-button") == 0)
    {
        GameEngine_quit(g_gameEngine);
    }
}

TitleMainMenu* TitleMainMenu_create(GameContext* context, TitleUIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;

    Input_clearUIInput(GameContext_getInput(context));

    TitleMainMenu* self = (TitleMainMenu*)calloc(1, sizeof(TitleMainMenu));
    ASSERT_NEW(self);

    self->m_uiManager = manager;
    self->m_context = context;
    self->m_mainPanel = UIObject_create(uiSystem, "main-panel");
    self->m_focusManager = UIFocusManager_create(uiSystem);

    UIObject_setParent(self->m_mainPanel, canvas);

    UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 4, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);
    UIGridLayout_setColumnSize(layout, 0, 150.f);

    font = AssetManager_getFont(assets, FONT_TITLE);
    UILabel* label = UILabel_create(uiSystem, "title-label", font);
    UILabel_setTextString(label, "SDL3 Template");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.white);

    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    const char* buttonNames[3] = { "start-button", "settings-button", "quit-button" };
    const char* buttonLabels[3] = { "Start", "Settings", "Quit" };
    font = AssetManager_getFont(assets, FONT_NORMAL);
    for (int i = 0; i < 3; i++)
    {
        UIButton* button = UIButton_create(uiSystem, buttonNames[i], font);
        UIButton_setLabelString(button, buttonLabels[i]);
        UIButton_setOnClickCallback(button, TitleMainMenu_onClick);
        UISelectable_setUserData(button, self);
        UIStyle_setDefaultButton(button, assets);
        UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
        UIFocusManager_addSelectable(self->m_focusManager, button);
        if (i == 0)
        {
            UIFocusManager_setFocused(self->m_focusManager, button);
        }
    }

    return self;
}

void TitleMainMenu_destroy(TitleMainMenu* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);
    UIFocusManager_destroy(self->m_focusManager);

    free(self);
}

void TitleMainMenu_update(TitleMainMenu* self)
{
    Input* input = GameContext_getInput(self->m_context);
    UIFocusManager_update(self->m_focusManager, &input->uiInput);
}
