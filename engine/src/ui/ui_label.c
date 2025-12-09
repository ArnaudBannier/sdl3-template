/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_label.h"
#include "game_engine_common.h"

UILabel* UILabel_create(const char* objectName, TTF_Font* font)
{
    UILabel* self = (UILabel*)calloc(1, sizeof(UILabel));
    AssertNew(self);

    UILabel_init(self, objectName, font);

    return self;
}

void UILabel_init(void* self, const char* objectName, TTF_Font* font)
{
    assert(self && "self must not be NULL");

    UIObject* selfObject = (UIObject*)self;
    UILabel* selfLabel = (UILabel*)self;
    UIObject_init(selfObject, objectName);

    selfObject->m_type |= UI_TYPE_LABEL;
    selfLabel->m_text = TTF_CreateText(g_textEngine, font, "Label", 0);
    selfLabel->m_textString = SDL_strdup("Label");
    selfLabel->m_color = g_colors.gray0;
    selfLabel->m_anchor = Vec2_anchor_center;

    // Virtual methods
    selfObject->m_onRender = UILabelVM_onRender;
    selfObject->m_onUpdate = UILabelVM_onUpdate;
    selfObject->m_onDestroy = UILabelVM_onDestroy;
}

void UILabelVM_onRender(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UILabel* selfLabel = (UILabel*)self;
    TTF_Text* ttfText = selfLabel->m_text;

    // Update text
    SDL_Color textColor = selfLabel->m_color;
    bool success = TTF_SetTextString(ttfText, selfLabel->m_textString, strlen(selfLabel->m_textString));
    assert(success);

    // Render text
    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(selfObj, &viewportRect);
    UIUtils_renderText(ttfText, &viewportRect, selfLabel->m_anchor, &textColor);
}

void UILabelVM_onDestroy(void* self)
{
    UILabel* selfLabel = (UILabel*)self;
    SDL_free(selfLabel->m_textString);
    TTF_DestroyText(selfLabel->m_text);

    UIObjectVM_onDestroy(self);
}
