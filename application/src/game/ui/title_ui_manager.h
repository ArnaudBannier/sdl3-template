/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct GameContext GameContext;
typedef struct TitleSettingsPage TitleSettingsPage;
typedef struct TitleMainMenu TitleMainMenu;

typedef enum TitleUIPageId
{
    TITLE_UI_PAGE_MAIN_MENU,
    TITLE_UI_PAGE_SETTINGS,
    //
    TITLE_UI_PAGE_COUNT
} TitleUIPageId;

typedef struct TitleUIManager
{
    GameContext* m_context;
    bool m_activePages[TITLE_UI_PAGE_COUNT];

    TitleMainMenu* m_mainMenu;
    TitleSettingsPage* m_settingsPage;
} TitleUIManager;

TitleUIManager* TitleUIManager_create(GameContext* context);
void TitleUIManager_destroy(TitleUIManager* self);
void TitleUIManager_update(TitleUIManager* self);

INLINE void TitleUIManager_openPage(TitleUIManager* self, TitleUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= TITLE_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->m_activePages[pageId] = true;
}

INLINE void TitleUIManager_closePage(TitleUIManager* self, TitleUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= TITLE_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->m_activePages[pageId] = false;
}

INLINE void TitleUIManager_closePages(TitleUIManager* self)
{
    assert(self && "self must not be NULL");
    for (int i = 0; i < TITLE_UI_PAGE_COUNT; ++i)
    {
        self->m_activePages[i] = false;
    }
}
