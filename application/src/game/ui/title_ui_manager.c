/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/title_ui_manager.h"
#include "game/ui/title_main_menu.h"
#include "game/ui/title_settings_page.h"
#include "game/core/game_context.h"
#include "game/game_config.h"

TitleUIManager* TitleUIManager_create(GameContext* context)
{
    TitleUIManager* self = (TitleUIManager*)calloc(1, sizeof(TitleUIManager));
    ASSERT_NEW(self);

    self->m_context = context;
    self->m_mainMenu = NULL;
    self->m_settingsPage = NULL;

    return self;
}

void TitleUIManager_destroy(TitleUIManager* self)
{
    if (!self) return;

    TitleMainMenu_destroy(self->m_mainMenu);
    TitleSettingsPage_destroy(self->m_settingsPage);
    free(self);
}

static void TitleUIManager_applyPageChange(TitleUIManager* self)
{
    // Title main menu
    if (self->m_activePages[TITLE_UI_PAGE_MAIN_MENU])
    {
        if (!self->m_mainMenu)
        {
            self->m_mainMenu = TitleMainMenu_create(self->m_context, self);
        }
    }
    else
    {
        TitleMainMenu_destroy(self->m_mainMenu);
        self->m_mainMenu = NULL;
    }

    // Settings page
    if (self->m_activePages[TITLE_UI_PAGE_SETTINGS])
    {
        if (!self->m_settingsPage)
        {
            self->m_settingsPage = TitleSettingsPage_create(self->m_context, self);
        }
    }
    else
    {
        TitleSettingsPage_destroy(self->m_settingsPage);
        self->m_settingsPage = NULL;
    }
}

void TitleUIManager_update(TitleUIManager* self)
{
    GameContext* context = self->m_context;
    Input* gameInput = GameContext_getInput(context);

    TitleUIManager_applyPageChange(self);

    if (self->m_mainMenu)
    {
        TitleMainMenu_update(self->m_mainMenu);
    }
    if (self->m_settingsPage)
    {
        TitleSettingsPage_update(self->m_settingsPage);
    }
}
