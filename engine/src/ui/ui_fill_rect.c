/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_fill_rect.h"
#include "game_engine_common.h"

UIFillRect* UIFillRect_create(const char* objectName, SDL_Color color)
{
    UIFillRect* self = (UIFillRect*)calloc(1, sizeof(UIFillRect));
    AssertNew(self);

    UIFillRect_init(self, objectName, color);

    return self;
}

void UIFillRect_init(void* self, const char* objectName, SDL_Color color)
{
    assert(self && "self must not be NULL");

    UIObject* selfObject = (UIObject*)self;
    UIFillRect* selfFillRect = (UIFillRect*)self;
    UIObject_init(selfObject, objectName);

    selfObject->m_type |= UI_TYPE_FILL_RECT;
    selfFillRect->m_color = color;
    selfFillRect->m_opacity = 1.f;

    // Virtual methods
    selfObject->m_onRender = UIFillRectVM_onRender;
    selfObject->m_onUpdate = UIFillRectVM_onUpdate;
    selfObject->m_onDestroy = UIFillRectVM_onDestroy;
}

void UIFillRectVM_onRender(void* self)
{
    UIFillRect* selfFillRect = (UIFillRect*)self;

    // Render image
    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(self, &viewportRect);
    SDL_Color color = selfFillRect->m_color;
    color.a = (Uint8)(color.a * selfFillRect->m_opacity);

    bool success = SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "UIFillRectVM_onRender");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }

    success = SDL_RenderFillRect(g_renderer, &viewportRect);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "UIFillRectVM_onRender");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }
}
