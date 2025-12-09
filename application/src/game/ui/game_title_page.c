/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/game_title_page.h"
#include "game/ui/game_ui_manager.h"
#include "game/scene.h"
#include "common/ui_style.h"

static void GameTitlePage_onClick(void* selectable)
{
    GameTitlePage* self = (GameTitlePage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    int action = UISelectable_getUserId(selectable);
    assert(action >= 0 && action < GAME_UI_ACTION_COUNT && "Invalid action id");

    self->m_nextAction = action;

    printf("Button clicked: %s\n", UIObject_getObjectName(selectable));
}

GameTitlePage* GameTitlePage_create(Scene* scene, GameUIManager *manager)
{
    UICanvas* canvas = manager->m_canvas;
    AssetManager* assets = Scene_getAssetManager(scene);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;

    GameTitlePage* self = (GameTitlePage*)calloc(1, sizeof(GameTitlePage));
    AssertNew(self);

    self->m_manager = manager;
    self->m_scene = scene;
    self->m_mainPanel = UIObject_create("main-panel");
    self->m_nextAction = GAME_UI_ACTION_NONE;
    self->m_focusManager = UIFocusManager_create();
    UIFocusManager_setCanvas(self->m_focusManager, canvas);

    UIObject_setParent(self->m_mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.5f, 0.0f);
    rect.anchorMax = Vec2_set(0.5f, 1.0f);
    rect.offsetMin = Vec2_set(-75.f, 0.0f);
    rect.offsetMax = Vec2_set(+75.f, 0.0f);
    UIObject_setRect(self->m_mainPanel, rect);

    UIGridLayout* layout = UIGridLayout_create("main-layout", 4, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);

    font = AssetManager_getFont(assets, FONT_BIG_BOLD);
    UILabel* label = UILabel_create("title-label", font);
    UILabel_setTextString(label, "SDL3 Template");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.orange2);

    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    const char* buttonNames[3] = { "start-button", "settings-button", "quit-button" };
    GameUIAction buttonActions[3] = {
        GAME_UI_ACTION_START,
        GAME_UI_ACTION_OPEN_SETTINGS,
        GAME_UI_ACTION_QUIT
    };
    const char* buttonLabels[3] = { "Start", "Settings", "Quit" };
    font = AssetManager_getFont(assets, FONT_NORMAL);
    for (int i = 0; i < 3; ++i)
    {
        UIButton* button = UIButton_create(buttonNames[i], font);
        UIButton_setLabelString(button, buttonLabels[i]);
        UIButton_setOnClickCallback(button, GameTitlePage_onClick);
        UISelectable_setUserData(button, self);
        UISelectable_setUserId(button, (int)buttonActions[i]);
        UIStyle_setDefaultButton(button);
        UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
        UIFocusManager_addSelectable(self->m_focusManager, button);
        if (i == 0)
        {
            UIFocusManager_setFocused(self->m_focusManager, button);
        }
    }

    return self;
}

void GameTitlePage_destroy(GameTitlePage* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);
    UIFocusManager_destroy(self->m_focusManager);

    free(self);
}

void GameTitlePage_update(GameTitlePage* self, UIInput* input)
{
    UIFocusManager_update(self->m_focusManager, input);

    switch (self->m_nextAction)
    {
    case GAME_UI_ACTION_START:
        self->m_manager->m_nextAction = GAME_UI_ACTION_START;
        break;
    case GAME_UI_ACTION_OPEN_SETTINGS:
        self->m_manager->m_nextAction = GAME_UI_ACTION_OPEN_SETTINGS;
        break;
    case GAME_UI_ACTION_QUIT:
        Scene_quit(self->m_scene);
        break;
    default:
        break;
    }
    self->m_nextAction = GAME_UI_ACTION_NONE;
}
