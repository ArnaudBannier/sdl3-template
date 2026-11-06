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

    selfObject->type |= UI_TYPE_LABEL;
    selfObject->gizmosColor = g_colors.teal5;
    selfObject->gizmosIsFilled = false;
    selfObject->gizmosOpacity = 1.f;
    selfLabel->textString = SDL_strdup("Label");
    ASSERT_NEW(selfLabel->textString);

    selfLabel->text.ttfText = TTF_CreateText(g_engine.sdl.textEngine, font, "Label", 0);
    selfLabel->text.color = g_colors.gray0;
    selfLabel->text.anchor = Vec2_anchor_center;
    ASSERT_NEW(selfLabel->text.ttfText);

    selfLabel->blendMod.alpha = 1.f;
    selfLabel->blendMod.mode = SDL_BLENDMODE_BLEND;

    // Virtual methods
    selfObject->onRender = UILabelVM_onRender;
    selfObject->onUpdate = UILabelVM_onUpdate;
    selfObject->onDestroy = UILabelVM_onDestroy;
    selfObject->onDrawGizmos = UIObjectVM_onDrawGizmos;
}

void UILabelVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem)
{
    UIObject* selfObj = (UIObject*)selfPtr;
    UILabel* selfLabel = (UILabel*)selfPtr;
    TTF_Text* ttfText = selfLabel->text.ttfText;

    // Update text
    bool success = TTF_SetTextString(ttfText, selfLabel->textString, strlen(selfLabel->textString));
    assert(success);

    // Render text
    Transform transform = { 0 };
    RenderDim dim = { 0 };
    RenderAnchor anchor = { 0 };

    UITransform_getComponents(&(selfObj->transform), &transform, &dim, &anchor);
    GraphicsCmd cmd = { 0 };
    cmd.sortingLayer = &(selfObj->layer);
    cmd.transform = &transform;
    cmd.dimensions = &dim;
    cmd.anchor = &anchor;
    cmd.text = &(selfLabel->text);
    cmd.blendMod = &(selfLabel->blendMod);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

void UILabelVM_onDestroy(void* selfPtr)
{
    UILabel* selfLabel = (UILabel*)selfPtr;
    SDL_free(selfLabel->textString);
    TTF_DestroyText(selfLabel->text.ttfText);

    UIObjectVM_onDestroy(selfPtr);
}
