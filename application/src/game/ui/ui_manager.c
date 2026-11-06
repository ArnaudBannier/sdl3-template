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

    self->context = context;
    self->mainMenu = NULL;
    self->settingsPage = NULL;

    return self;
}

void UIManager_destroy(UIManager* self)
{
    if (!self) return;

    MainMenu_destroy(self->mainMenu);
    SettingsPage_destroy(self->settingsPage);
    free(self);
}

static void UIManager_applyPageChange(UIManager* self)
{
    // Title main menu
    if (self->activePages[TITLE_UI_PAGE_MAIN_MENU])
    {
        if (!self->mainMenu)
        {
            self->mainMenu = MainMenu_create(self->context, self);
        }
    }
    else
    {
        MainMenu_destroy(self->mainMenu);
        self->mainMenu = NULL;
    }

    // Settings page
    if (self->activePages[TITLE_UI_PAGE_SETTINGS])
    {
        if (!self->settingsPage)
        {
            self->settingsPage = SettingsPage_create(self->context, self);
        }
    }
    else
    {
        SettingsPage_destroy(self->settingsPage);
        self->settingsPage = NULL;
    }
}

void UIManager_update(UIManager* self)
{
    GameContext* context = self->context;
    Input* gameInput = GameContext_getInput(context);

    UIManager_applyPageChange(self);

    if (self->mainMenu)
    {
        MainMenu_update(self->mainMenu);
    }
    if (self->settingsPage)
    {
        SettingsPage_update(self->settingsPage);
    }
}
