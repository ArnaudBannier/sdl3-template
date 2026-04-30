/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/ui_style.h"
#include "common/assets.h"
#include "common/common.h"

void UIStyle_setDefaultButton(UIButton* button, AssetManager* assets)
{
    UIButton_setLabelColor(button, UI_BUTTON_STATE_NORMAL,  g_colors.orange1);
    UIButton_setLabelColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.orange4);
    UIButton_setLabelColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange5);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_NORMAL, g_colors.orange1);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.orange4);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange5);

    UISelectable_setAudioOnFocused(button, AUDIO_UI_SELECT);
    UISelectable_setAudioOnClick(button, AUDIO_UI_VALIDATE);
    UIButton_setUseColorMod(button, true);

    SpriteSheet* uiSpriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    SpriteGroup* buttonGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "button");

    UIButton_setSpriteGroup(button, buttonGroup);
    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_NORMAL, 0);
    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_FOCUSED, 1);
    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_PRESSED, 2);

    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_ACTIVE, 0);
    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_ACTIVE_FOCUSED, 1);
    UIButton_setSpriteIndex(button, UI_BUTTON_STATE_ACTIVE_PRESSED, 2);
}

void UIStyle_setDefaultList(UIList* list, AssetManager *assets)
{
    SpriteSheet* uiSpriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    SpriteGroup* buttonGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "button");

    UIList_setSpriteGroup(list, buttonGroup);
    UIList_setSpriteIndex(list, UI_LIST_STATE_NORMAL, 0);
    UIList_setSpriteIndex(list, UI_LIST_STATE_FOCUSED, 1);
    UIList_setSpriteIndex(list, UI_LIST_STATE_ACTIVE, 2);
    UIRect labelRect = { 0 };
    labelRect.anchorMin = Vec2_set(0.0f, 0.0f);
    labelRect.anchorMax = Vec2_set(0.5f, 1.0f);
    labelRect.offsetMin = Vec2_set(+15.f, 0.f);
    labelRect.offsetMax = Vec2_set(0.f, 0.f);
    UIList_setLabelRect(list, labelRect);

    UIList_setBackgroundColor(list, UI_LIST_STATE_NORMAL, g_colors.orange1);
    UIList_setBackgroundColor(list, UI_LIST_STATE_FOCUSED, g_colors.orange4);
    UIList_setBackgroundColor(list, UI_LIST_STATE_ACTIVE, g_colors.orange5);
    UIList_setUseColorMod(list, true);

    UIList_setLabelColor(list, UI_LIST_STATE_NORMAL, g_colors.orange1);
    UIList_setLabelColor(list, UI_LIST_STATE_FOCUSED, g_colors.orange4);
    UIList_setLabelColor(list, UI_LIST_STATE_ACTIVE, g_colors.orange5);

    UIList_setItemColor(list, UI_LIST_STATE_NORMAL, g_colors.orange1);
    UIList_setItemColor(list, UI_LIST_STATE_FOCUSED, g_colors.white);
    UIList_setItemColor(list, UI_LIST_STATE_ACTIVE, g_colors.white);

    UISelectable_setAudioOnFocused(list, AUDIO_UI_SELECT);
    UISelectable_setAudioOnClick(list, AUDIO_UI_VALIDATE);

    UIButton* prevButton = UIList_getPrevButton(list);
    UIButton* nextButton = UIList_getNextButton(list);
    UIButton_setLabelString(prevButton, "<");
    UIButton_setLabelString(nextButton, ">");

    TTF_Font* font = AssetManager_getFont(assets, FONT_BOLD);
    UIButton* buttons[] = { prevButton, nextButton };
    for (int i = 0; i < 2; i++)
    {
        UIButton* button = buttons[i];
        UIButton_setFont(button, font);
        UIButton_setUseColorMod(button, true);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_NORMAL, g_colors.transparent);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.orange4);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange5);
        UIButton_setLabelColor(button, UI_BUTTON_STATE_NORMAL, g_colors.orange5);
        UIButton_setLabelColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.white);
        UIButton_setLabelColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange6);
    }
}
