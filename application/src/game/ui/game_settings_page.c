/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/game_settings_page.h"
#include "game/ui/game_ui_manager.h"
#include "game/scene.h"
#include "game/game_config.h"
#include "common/ui_style.h"

static void GameSettingsPage_onClick(void* selectable)
{
    GameSettingsPage* self = (GameSettingsPage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    int action = UISelectable_getUserId(selectable);
    assert(action >= 0 && action < GAME_UI_ACTION_COUNT && "Invalid action id");
    self->m_nextAction = action;

    printf("Button clicked: %s\n", UIObject_getObjectName(selectable));
}

static void GameSettingsPage_onItemChanged(void* selectable, int currItemIdx, int prevItemIdx, bool increase)
{
    GameSettingsPage* self = (GameSettingsPage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    printf("List item changed: %s\n", UIObject_getObjectName(selectable));
}

GameSettingsPage* GameSettingsPage_create(Scene* scene, GameUIManager* manager)
{
    UICanvas* canvas = manager->m_canvas;
    AssetManager* assets = Scene_getAssetManager(scene);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;
    char buffer[128] = { 0 };

    GameSettingsPage* self = (GameSettingsPage*)calloc(1, sizeof(GameSettingsPage));
    AssertNew(self);

    self->m_scene = scene;
    self->m_manager = manager;
    self->m_mainPanel = UIObject_create("main-panel");
    self->m_nextAction = GAME_UI_ACTION_NONE;
    self->m_focusManager = UIFocusManager_create();
    UIFocusManager_setCanvas(self->m_focusManager, canvas);

    UIObject_setParent(self->m_mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.25f, 0.0f);
    rect.anchorMax = Vec2_set(0.75f, 1.0f);
    rect.offsetMin = Vec2_zero;
    rect.offsetMax = Vec2_zero;
    UIObject_setRect(self->m_mainPanel, rect);

    UIGridLayout* layout = UIGridLayout_create("main-layout", 4, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);
    UIGridLayout_setRowSpacing(layout, 2, 20.f);

    // Title
    font = AssetManager_getFont(assets, FONT_BIG_BOLD);
    UILabel* label = UILabel_create("title-label", font);
    UILabel_setTextString(label, "Settings");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.orange2);
    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    font = AssetManager_getFont(assets, FONT_NORMAL);

    const char *toggleItems[] = { "OFF", "ON" };
    const int toggleItemCount = sizeof(toggleItems) / sizeof(toggleItems[0]);

    UIList* fullscreenList = UIList_create(
        "fullscreen-list", font, toggleItemCount,
        UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->m_fullscreenList = fullscreenList;

    UIList_setLabelString(fullscreenList, "Fullscreen");
    for (int i = 0; i < toggleItemCount; ++i)
    {
        UIList_setItemString(fullscreenList, i, toggleItems[i]);
    }
    UIStyle_setDefaultList(fullscreenList, assets);
    UISelectable_setUserData(fullscreenList, self);
    UISelectable_setUserId(fullscreenList, GAME_UI_ACTION_CHANGE_FULLSCREEN);
    UIList_setOnItemChangedCallback(fullscreenList, GameSettingsPage_onItemChanged);

    SDL_WindowFlags windowFlags = SDL_GetWindowFlags(g_window);
    if (windowFlags & SDL_WINDOW_FULLSCREEN)
    {
        UIList_setSelectedItem(fullscreenList, 1);
    }
    else
    {
        UIList_setSelectedItem(fullscreenList, 0);
    }

    UIGridLayout_addObject(layout, fullscreenList, 1, 0, 1, 1);
    UIFocusManager_addSelectable(self->m_focusManager, fullscreenList);

    // Show elapsed list
    UIList* elapsedList = UIList_create(
        "elapsed-list", font, toggleItemCount,
        UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->m_elapsedList = elapsedList;

    UIList_setLabelString(elapsedList, "Show elapsed time");
    for (int i = 0; i < toggleItemCount; ++i)
    {
        UIList_setItemString(elapsedList, i, toggleItems[i]);
    }
    UIList_setSelectedItem(elapsedList, (int)g_gameConfig.showElapsedTime);
    UIStyle_setDefaultList(elapsedList, assets);
    UISelectable_setUserData(elapsedList, self);
    UISelectable_setUserId(elapsedList, 0);
    UIGridLayout_addObject(layout, elapsedList, 2, 0, 1, 1);
    UIFocusManager_addSelectable(self->m_focusManager, elapsedList);

    // Button layout
    UIGridLayout* buttonlayout = UIGridLayout_create("button-layout", 1, 2);
    UIGridLayout_setColumnSizes(buttonlayout, 100.f);
    UIGridLayout_setColumnSpacings(buttonlayout, 5.f);
    UIGridLayout_addObject(layout, buttonlayout, 3, 0, 1, 1);

    // Apply button
    UIButton* applyButton = UIButton_create("apply-button", font);
    UIButton_setLabelString(applyButton, "Apply settings");
    UIButton_setOnClickCallback(applyButton, GameSettingsPage_onClick);
    UISelectable_setUserData(applyButton, self);
    UISelectable_setUserId(applyButton, GAME_UI_ACTION_APPLY_SETTINGS);
    UIStyle_setDefaultButton(applyButton);

    UIGridLayout_addObject(buttonlayout, applyButton, 0, 0, 1, 1);
    UIFocusManager_addSelectable(self->m_focusManager, applyButton);

    // Back button
    UIButton* backButton = UIButton_create("back-button", font);
    UIButton_setLabelString(backButton, "Back");
    UIButton_setOnClickCallback(backButton, GameSettingsPage_onClick);
    UISelectable_setUserData(backButton, self);
    UISelectable_setUserId(backButton, GAME_UI_ACTION_OPEN_MAIN);
    UIStyle_setDefaultButton(backButton);

    UIGridLayout_addObject(buttonlayout, backButton, 0, 1, 1, 1);
    UIFocusManager_addSelectable(self->m_focusManager, backButton);

    UIFocusManager_setFocused(self->m_focusManager, fullscreenList);

    return self;
}

void GameSettingsPage_destroy(GameSettingsPage* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);
    UIFocusManager_destroy(self->m_focusManager);

    free(self);
}

void GameSettingsPage_update(GameSettingsPage* self, UIInput* input)
{
    UIFocusManager_update(self->m_focusManager, input);

    switch (self->m_nextAction)
    {
    case GAME_UI_ACTION_OPEN_MAIN:
        self->m_manager->m_nextAction = GAME_UI_ACTION_OPEN_MAIN;
        break;

    case GAME_UI_ACTION_APPLY_SETTINGS:
    {
        int itemIndex = UIList_getSelectedItem(self->m_fullscreenList);
        SDL_SetWindowFullscreen(g_window, itemIndex == 1);
        g_gameConfig.showElapsedTime = (bool)UIList_getSelectedItem(self->m_elapsedList);
        self->m_manager->m_nextAction = GAME_UI_ACTION_OPEN_MAIN;
        break;
    }
    default:
        break;
    }
    self->m_nextAction = GAME_UI_ACTION_NONE;
}
