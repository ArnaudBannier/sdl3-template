/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/ui_style.h"
#include "common/assets.h"
#include "common/common.h"

void UIStyle_setDefaultButton(UIButton* button)
{
    UIButton_setLabelColor(button, UI_BUTTON_STATE_NORMAL,  g_colors.orange5);
    UIButton_setLabelColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.orange4);
    UIButton_setLabelColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange6);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_NORMAL, g_colors.gray8);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.gray7);
    UIButton_setBackgroundColor(button, UI_BUTTON_STATE_PRESSED, Color_lerp(g_colors.gray8, g_colors.gray9, 0.5f));
}

void UIStyle_setDefaultList(UIList* list, AssetManager *assets)
{
    //SpriteSheet* uiSpriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    //SpriteGroup* buttonGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "button");
    //SpriteGroup* arrowGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "arrows");

    //UIList_setSpriteGroup(list, buttonGroup);
    //UIList_setSpriteIndex(list, UI_LIST_STATE_NORMAL, 0);
    //UIList_setSpriteIndex(list, UI_LIST_STATE_FOCUSED, 1);
    //UIList_setSpriteIndex(list, UI_LIST_STATE_ACTIVE, 2);
    UIRect labelRect = { 0 };
    labelRect.anchorMin = Vec2_set(0.0f, 0.0f);
    labelRect.anchorMax = Vec2_set(0.5f, 1.0f);
    labelRect.offsetMin = Vec2_set(+10.f, 0.f);
    labelRect.offsetMax = Vec2_set(0.f, 0.f);
    UIList_setLabelRect(list, labelRect);

    UIList_setBackgroundColor(list, UI_LIST_STATE_NORMAL, g_colors.gray8);
    UIList_setBackgroundColor(list, UI_LIST_STATE_FOCUSED, g_colors.gray7);
    UIList_setBackgroundColor(list, UI_LIST_STATE_ACTIVE, Color_lerp(g_colors.gray8, g_colors.gray9, 0.5f));

    UIButton* prevButton = UIList_getPrevButton(list);
    UIButton* nextButton = UIList_getNextButton(list);
    UIRect symbolRect = { 0 };
    symbolRect.anchorMin = Vec2_set(0.5f, 0.5f);
    symbolRect.anchorMax = Vec2_set(0.5f, 0.5f);
    symbolRect.offsetMin = Vec2_set(-5.f, -5.f);
    symbolRect.offsetMax = Vec2_set(+5.f, +5.f);
    //UIButton_setSymbolSprite(prevButton, arrowGroup, 0, symbolRect);
    //UIButton_setSymbolSprite(nextButton, arrowGroup, 1, symbolRect);

    UIButton_setLabelString(prevButton, "<");
    UIButton_setLabelString(nextButton, ">");
    //UIButton_setLabelString(prevButton, "");
    //UIButton_setLabelString(nextButton, "");

    TTF_Font* font = AssetManager_getFont(assets, FONT_BOLD);

    UIButton* buttons[] = { prevButton, nextButton };
    for (int i = 0; i < 2; ++i)
    {
        UIButton* button = buttons[i];
        UIButton_setFont(button, font);
        UIButton_setUseColorMod(button, true);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_NORMAL, g_colors.transparent);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.gray6);
        UIButton_setBackgroundColor(button, UI_BUTTON_STATE_PRESSED, Color_lerp(g_colors.gray8, g_colors.gray9, 0.5f));
        UIButton_setLabelColor(button, UI_BUTTON_STATE_NORMAL, g_colors.orange5);
        UIButton_setLabelColor(button, UI_BUTTON_STATE_FOCUSED, g_colors.orange4);
        UIButton_setLabelColor(button, UI_BUTTON_STATE_PRESSED, g_colors.orange6);
    }
}
