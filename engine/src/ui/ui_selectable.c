/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_selectable.h"
#include "game_engine_common.h"

UISelectable* UISelectable_create(const char* objectName)
{
    UISelectable* self = (UISelectable*)calloc(1, sizeof(UISelectable));
    AssertNew(self);

    UISelectable_init(self, objectName);

    return self;
}

void UISelectable_init(void* self, const char* objectName)
{
    assert(self && "self must not be NULL");

    UIObject* selfObject = (UIObject*)self;
    UISelectable* selfSelectable = (UISelectable*)self;
    UIObject_init(selfObject, objectName);

    selfObject->m_type |= UI_TYPE_SELECTABLE;

    selfSelectable->m_handledActionFlags = 0;
    selfSelectable->m_focusState = UI_FOCUS_STATE_NORMAL;

    // Virtual methods
    selfObject->m_onRender = UISelectableVM_onRender;
    selfObject->m_onUpdate = UISelectableVM_onUpdate;
    selfObject->m_onDestroy = UISelectableVM_onDestroy;

    selfSelectable->m_onFocusChanged = UISelectableVM_onFocusChanged;
    selfSelectable->m_onFocus = NULL;
    selfSelectable->m_focusState = UI_FOCUS_STATE_NORMAL;
}

void UISelectable_setFocusState(void* self, UIFocusState state)
{
    UISelectable* selfSelectable = (UISelectable*)self;
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");

    if (selfSelectable->m_focusState == state) return;

    int prevState = selfSelectable->m_focusState;
    selfSelectable->m_focusState = state;

    selfSelectable->m_onFocusChanged(selfSelectable, state, prevState);
    if (selfSelectable->m_userOnFocusChanged)
    {
        selfSelectable->m_userOnFocusChanged(selfSelectable, state, prevState);
    }
}

void UISelectableVM_onRender(void* self)
{
    UIObject* selfObj = (UIObject*)self;
    UISelectable* selfSelectable = (UISelectable*)self;

    switch (selfSelectable->m_focusState)
    {
    default:
    case UI_FOCUS_STATE_NORMAL:
        SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
        break;

    case UI_FOCUS_STATE_FOCUSED:
        SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
        break;
    }

    SDL_FRect viewportRect = { 0 };
    UIObject_getViewportRect(selfObj, &viewportRect);
    SDL_RenderFillRect(g_renderer, &viewportRect);
}

void UISelectableVM_onFocusChanged(void* self, UIFocusState currState, UIFocusState prevState)
{
    // Nothing to do
}

void UISelectableVM_onFocus(void* self, UIInput* input)
{
    // Nothing to do
}
