/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/ui_manager.h"
#include "game/ui/main_menu.h"
#include "game/ui/settings_page.h"
#include "game/core/game_context.h"
#include "game/game_config.h"

UIManager* UIManager_create(GameContext* context)
{
    UIManager* self = (UIManager*)calloc(1, sizeof(UIManager));
    ASSERT_NEW(self);

    self->m_context = context;
    self->m_mainMenu = NULL;
    self->m_settingsPage = NULL;

    return self;
}

void UIManager_destroy(UIManager* self)
{
    if (!self) return;

    MainMenu_destroy(self->m_mainMenu);
    SettingsPage_destroy(self->m_settingsPage);
    free(self);
}

static void UIManager_applyPageChange(UIManager* self)
{
    // Title main menu
    if (self->m_activePages[TITLE_UI_PAGE_MAIN_MENU])
    {
        if (!self->m_mainMenu)
        {
            self->m_mainMenu = MainMenu_create(self->m_context, self);
        }
    }
    else
    {
        MainMenu_destroy(self->m_mainMenu);
        self->m_mainMenu = NULL;
    }

    // Settings page
    if (self->m_activePages[TITLE_UI_PAGE_SETTINGS])
    {
        if (!self->m_settingsPage)
        {
            self->m_settingsPage = SettingsPage_create(self->m_context, self);
        }
    }
    else
    {
        SettingsPage_destroy(self->m_settingsPage);
        self->m_settingsPage = NULL;
    }
}

void UIManager_update(UIManager* self)
{
    GameContext* context = self->m_context;
    Input* gameInput = GameContext_getInput(context);

    UIManager_applyPageChange(self);

    if (self->m_mainMenu)
    {
        MainMenu_update(self->m_mainMenu);
    }
    if (self->m_settingsPage)
    {
        SettingsPage_update(self->m_settingsPage);
    }
}
