/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/settings_page.h"
#include "game/ui/ui_manager.h"
#include "game/core/game_context.h"
#include "game/game_config.h"
#include "common/ui_style.h"

static void SettingsPage_onClick(void* selectable)
{
    SettingsPage* self = (SettingsPage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    const char* objectName = UIObject_getObjectName(selectable);
    if (strcmp(objectName, "back-button") == 0)
    {
        // Restore previous audio settings when going back to main page
        GameContext* context = self->context;
        AudioSystem* audioSystem = GameContext_getAudioSystem(context);
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, self->prevMusicGain);
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_SFX, self->prevSfxGain);
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_UI, self->prevUIGain);
        g_gameConfig.musicGain = self->prevMusicGain;
        g_gameConfig.sfxGain = self->prevSfxGain;
        g_gameConfig.uiGain = self->prevUIGain;

        UIManager_closePages(self->uiManager);
        UIManager_openPage(self->uiManager, TITLE_UI_PAGE_MAIN_MENU);
    }
    else if (strcmp(objectName, "apply-button") == 0)
    {
        int itemIndex = UIList_getSelectedItem(self->fullscreenList);
        SDL_SetWindowFullscreen(g_engine.sdl.window, itemIndex == 1);

        UIManager_closePages(self->uiManager);
        UIManager_openPage(self->uiManager, TITLE_UI_PAGE_MAIN_MENU);
    }
}

static void SettingsPage_onItemChanged(void* selectable, int currItemIdx, int prevItemIdx, bool increase)
{
    SettingsPage* self = (SettingsPage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    GameContext* context = self->context;
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);

    const char* objectName = UIObject_getObjectName(selectable);
    if (strcmp(objectName, "music-gain-list") == 0)
    {
        g_gameConfig.musicGain = currItemIdx / 10.f;
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, g_gameConfig.musicGain);
    }
    else if (strcmp(objectName, "sfx-gain-list") == 0)
    {
        g_gameConfig.sfxGain = currItemIdx / 10.f;
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_SFX, g_gameConfig.sfxGain);
    }
    else if (strcmp(objectName, "ui-gain-list") == 0)
    {
        g_gameConfig.uiGain = currItemIdx / 10.f;
        AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_UI, g_gameConfig.uiGain);
    }
}

SettingsPage* SettingsPage_create(GameContext* context, UIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;
    char buffer[128] = { 0 };

    Input_clearUIInput(GameContext_getInput(context));

    SettingsPage* self = (SettingsPage*)calloc(1, sizeof(SettingsPage));
    ASSERT_NEW(self);

    self->context = context;
    self->uiManager = manager;
    self->mainPanel = UIObject_create(uiSystem, "main-panel");
    self->focusManager = UIFocusManager_create(uiSystem);

    self->prevMusicGain = g_gameConfig.musicGain;
    self->prevSfxGain = g_gameConfig.sfxGain;
    self->prevUIGain = g_gameConfig.uiGain;

    UIObject_setParent(self->mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.25f, 0.0f);
    rect.anchorMax = Vec2_set(0.75f, 1.0f);
    rect.offsetMin = Vec2_zero;
    rect.offsetMax = Vec2_zero;
    UIObject_setRect(self->mainPanel, rect);

    UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 6, 1);
    UIObject_setParent(layout, self->mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);
    UIGridLayout_setRowSpacing(layout, 4, 20.f);

    int rowIndex = 0;

    // Title
    font = AssetManager_getFont(assets, FONT_TITLE);
    UILabel* label = UILabel_create(uiSystem, "title-label", font);
    UILabel_setTextString(label, "Settings");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.white);
    UIGridLayout_addObject(layout, label, rowIndex++, 0, 1, 1);

    font = AssetManager_getFont(assets, FONT_NORMAL);

    // Toggle items
    const char* toggleItems[] = { "OFF", "ON" };
    const int toggleItemCount = sizeof(toggleItems) / sizeof(toggleItems[0]);

    // Fullscreen list
    UIList* fullscreenList = UIList_create(
        uiSystem, "fullscreen-list", font, toggleItemCount,
        UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->fullscreenList = fullscreenList;

    UIList_setLabelString(fullscreenList, "Fullscreen");
    for (int i = 0; i < toggleItemCount; i++)
    {
        UIList_setItemString(fullscreenList, i, toggleItems[i]);
    }
    UIStyle_setDefaultList(fullscreenList, assets);
    UISelectable_setUserData(fullscreenList, self);
    UIList_setOnItemChangedCallback(fullscreenList, SettingsPage_onItemChanged);

    SDL_WindowFlags windowFlags = SDL_GetWindowFlags(g_engine.sdl.window);
    if (windowFlags & SDL_WINDOW_FULLSCREEN)
    {
        UIList_setSelectedItem(fullscreenList, 1);
    }
    else
    {
        UIList_setSelectedItem(fullscreenList, 0);
    }

    UIGridLayout_addObject(layout, fullscreenList, rowIndex++, 0, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, fullscreenList);

    // Volume lists
    const char* gainItems[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
    const int gainItemCount = sizeof(gainItems) / sizeof(gainItems[0]);

    // Music gain list
    UIList* musicGainList = UIList_create(
        uiSystem, "music-gain-list", font, 11,
        UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->musicGainList = musicGainList;

    UIList_setLabelString(musicGainList, "Music volume");
    for (int i = 0; i < gainItemCount; i++)
    {
        UIList_setItemString(musicGainList, i, gainItems[i]);
    }
    int selectedMusicGain = (int)roundf(g_gameConfig.musicGain * 10);
    selectedMusicGain = Int_clamp(selectedMusicGain, 0, gainItemCount - 1);
    UIList_setSelectedItem(musicGainList, selectedMusicGain);
    UIStyle_setDefaultList(musicGainList, assets);
    UISelectable_setUserData(musicGainList, self);
    UIList_setOnItemChangedCallback(musicGainList, SettingsPage_onItemChanged);

    UIGridLayout_addObject(layout, musicGainList, rowIndex++, 0, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, musicGainList);

    // SFX gain list
    UIList* sfxGainList = UIList_create(
        uiSystem, "sfx-gain-list", font, gainItemCount,
        UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->sfxGainList = sfxGainList;

    UIList_setLabelString(sfxGainList, "SFX volume");
    for (int i = 0; i < gainItemCount; i++)
    {
        UIList_setItemString(sfxGainList, i, gainItems[i]);
    }
    int selectedSfxGain = (int)roundf(g_gameConfig.sfxGain * 10);
    selectedSfxGain = Int_clamp(selectedSfxGain, 0, gainItemCount - 1);
    UIList_setSelectedItem(sfxGainList, selectedSfxGain);
    UIStyle_setDefaultList(sfxGainList, assets);
    UISelectable_setUserData(sfxGainList, self);
    UIList_setOnItemChangedCallback(sfxGainList, SettingsPage_onItemChanged);

    UIGridLayout_addObject(layout, sfxGainList, rowIndex++, 0, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, sfxGainList);

    // UI gain list
    UIList* uiGainList = UIList_create(
        uiSystem, "ui-gain-list", font, gainItemCount,
        UI_LIST_CONFIG_AUTO_NAVIGATION
    );
    self->uiGainList = uiGainList;

    UIList_setLabelString(uiGainList, "UI volume");
    for (int i = 0; i < gainItemCount; i++)
    {
        UIList_setItemString(uiGainList, i, gainItems[i]);
    }
    int selectedUiGain = (int)roundf(g_gameConfig.uiGain * 10);
    selectedUiGain = Int_clamp(selectedUiGain, 0, gainItemCount - 1);
    UIList_setSelectedItem(uiGainList, selectedUiGain);
    UIStyle_setDefaultList(uiGainList, assets);
    UISelectable_setUserData(uiGainList, self);
    UIList_setOnItemChangedCallback(uiGainList, SettingsPage_onItemChanged);

    UIGridLayout_addObject(layout, uiGainList, rowIndex++, 0, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, uiGainList);

    // Button layout
    UIGridLayout* buttonlayout = UIGridLayout_create(uiSystem, "button-layout", 1, 2);
    UIGridLayout_setColumnSizes(buttonlayout, 100.f);
    UIGridLayout_setColumnSpacings(buttonlayout, 20.f);
    UIGridLayout_addObject(layout, buttonlayout, rowIndex++, 0, 1, 1);

    // Apply button
    UIButton* applyButton = UIButton_create(uiSystem, "apply-button", font);
    UIButton_setLabelString(applyButton, "Apply settings");
    UIButton_setOnClickCallback(applyButton, SettingsPage_onClick);
    UISelectable_setUserData(applyButton, self);
    UIStyle_setDefaultButton(applyButton, assets);

    UIGridLayout_addObject(buttonlayout, applyButton, 0, 0, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, applyButton);

    // Back button
    UIButton* backButton = UIButton_create(uiSystem, "back-button", font);
    UIButton_setLabelString(backButton, "Back");
    UIButton_setOnClickCallback(backButton, SettingsPage_onClick);
    UISelectable_setUserData(backButton, self);
    UIStyle_setDefaultButton(backButton, assets);

    UIGridLayout_addObject(buttonlayout, backButton, 0, 1, 1, 1);
    UIFocusManager_addSelectable(self->focusManager, backButton);

    UIFocusManager_setFocused(self->focusManager, fullscreenList);

    return self;
}

void SettingsPage_destroy(SettingsPage* self)
{
    if (!self) return;

    UIObject_destroy(self->mainPanel);
    UIFocusManager_destroy(self->focusManager);

    free(self);
}

void SettingsPage_update(SettingsPage* self)
{
    Input* input = GameContext_getInput(self->context);
    UIFocusManager_update(self->focusManager, &input->uiInput);
}
