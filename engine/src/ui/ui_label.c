/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_label.h"
#include "engine_common.h"

UILabel* UILabel_create(UISystem* uiSystem, const char* objectName, TTF_Font* font)
{
    UILabel* self = (UILabel*)calloc(1, sizeof(UILabel));
    ASSERT_NEW(self);

    UILabel_init(self, uiSystem, objectName, font);

    return self;
}

void UILabel_init(void* selfPtr, UISystem* uiSystem, const char* objectName, TTF_Font* font)
{
    assert(selfPtr && "self must not be NULL");

    UIObject* selfObject = (UIObject*)selfPtr;
    UILabel* selfLabel = (UILabel*)selfPtr;
    UIObject_init(selfObject, uiSystem, objectName);

    selfObject->m_type |= UI_TYPE_LABEL;
    selfObject->m_gizmosColor = g_colors.teal5;
    selfObject->m_gizmosIsFilled = false;
    selfObject->m_gizmosOpacity = 1.f;
    selfLabel->m_textString = SDL_strdup("Label");
    ASSERT_NEW(selfLabel->m_textString);

    selfLabel->m_text.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "Label", 0);
    selfLabel->m_text.color = g_colors.gray0;
    selfLabel->m_text.anchor = Vec2_anchor_center;
    ASSERT_NEW(selfLabel->m_text.ttfText);

    selfLabel->m_blendMod.alpha = 1.f;
    selfLabel->m_blendMod.mode = SDL_BLENDMODE_BLEND;

    // Virtual methods
    selfObject->m_onRender = UILabelVM_onRender;
    selfObject->m_onUpdate = UILabelVM_onUpdate;
    selfObject->m_onDestroy = UILabelVM_onDestroy;
    selfObject->m_onDrawGizmos = UIObjectVM_onDrawGizmos;
}

void UILabelVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UILabel* selfLabel = (UILabel*)selfPtr;
    TTF_Text* ttfText = selfLabel->m_text.ttfText;

    // Update text
    bool success = TTF_SetTextString(ttfText, selfLabel->m_textString, strlen(selfLabel->m_textString));
    assert(success);

    // Render text
    Transform transform = { 0 };
    RenderDim dim = { 0 };
    RenderAnchor anchor = { 0 };

    UITransform_getComponents(&(selfObj->m_transform), &transform, &dim, &anchor);
    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->m_layer);
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.anchor = &anchor;
    cmd.text = &(selfLabel->m_text);
    cmd.blendMod = &(selfLabel->m_blendMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

void UILabelVM_onDestroy(void* selfPtr)
{
    UILabel* selfLabel = (UILabel*)selfPtr;
    SDL_free(selfLabel->m_textString);
    TTF_DestroyText(selfLabel->m_text.ttfText);

    UIObjectVM_onDestroy(selfPtr);
}
