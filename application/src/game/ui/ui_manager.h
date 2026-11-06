/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct GameContext GameContext;
typedef struct SettingsPage SettingsPage;
typedef struct MainMenu MainMenu;

typedef enum TitleUIPageId
{
    TITLE_UI_PAGE_MAIN_MENU,
    TITLE_UI_PAGE_SETTINGS,
    //
    TITLE_UI_PAGE_COUNT
} TitleUIPageId;

typedef struct UIManager
{
    GameContext* context;
    bool activePages[TITLE_UI_PAGE_COUNT];

    MainMenu* mainMenu;
    SettingsPage* settingsPage;
} UIManager;

UIManager* UIManager_create(GameContext* context);
void UIManager_destroy(UIManager* self);
void UIManager_update(UIManager* self);

INLINE void UIManager_openPage(UIManager* self, TitleUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= TITLE_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->activePages[pageId] = true;
}

INLINE void UIManager_closePage(UIManager* self, TitleUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= TITLE_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->activePages[pageId] = false;
}

INLINE void UIManager_closePages(UIManager* self)
{
    assert(self && "self must not be NULL");
    for (int i = 0; i < TITLE_UI_PAGE_COUNT; ++i)
    {
        self->activePages[i] = false;
    }
}
