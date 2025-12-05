/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "core/sprite_sheet.h"
#include "utils/utils.h"

typedef enum UIFocusState
{
    UI_FOCUS_STATE_NORMAL = 0,
    UI_FOCUS_STATE_FOCUSED,
    UI_FOCUS_STATE_DISABLED,
    UI_FOCUS_STATE_COUNT
} UIFocusState;

void UISelectableVM_onRender(void* self);
void UISelectableVM_onFocusChanged(void* self, UIFocusState currState, UIFocusState prevState);
void UISelectableVM_onFocus(void* self, UIInput* input);
#define UISelectableVM_onDestroy UIObjectVM_onDestroy
#define UISelectableVM_onUpdate UIObjectVM_onUpdate

typedef struct UISelectable
{
    UIObject m_baseObject;

    UIFocusState m_focusState;
    int m_handledActionFlags;

    int m_userId;
    void* m_userData;

    void (*m_onFocusChanged)(void* self, UIFocusState currState, UIFocusState prevState);
    void (*m_userOnFocusChanged)(void* self, UIFocusState currState, UIFocusState prevState);

    void (*m_onFocus)(void* self, UIInput* input);
    void (*m_userOnFocus)(void* self, UIInput* input);

} UISelectable;

UISelectable* UISelectable_create(const char* objectName);
void UISelectable_init(void* self, const char* objectName);
INLINE void UISelectable_destroy(void* self)
{
    UIObject_destroy(self);
}

INLINE void UISelectable_setHandledActions(void* self, int actionFlags)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    selfSelectable->m_handledActionFlags = actionFlags;
}

INLINE bool UISelectable_shouldHandleAction(void* self, int actionFlags)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    return ((selfSelectable->m_handledActionFlags & actionFlags) != 0);
}

void UISelectable_setFocusState(void* self, UIFocusState state);

INLINE UIFocusState UISelectable_getFocusState(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    return selfSelectable->m_focusState;
}

INLINE void UISelectable_setUserData(void* self, void* userData)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    selfSelectable->m_userData = userData;
}

INLINE void* UISelectable_getUserData(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    return selfSelectable->m_userData;
}

INLINE void UISelectable_setUserId(void* self, int userId)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    selfSelectable->m_userId = userId;
}

INLINE int UISelectable_getUserId(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    return selfSelectable->m_userId;
}

INLINE void UISelectable_setOnFocusChangedCallback(
    void* self,
    void (*onFocusChanged)(void* self, UIFocusState currState, UIFocusState prevState))
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    selfSelectable->m_onFocusChanged = onFocusChanged;
}

INLINE void UISelectable_onFocus(void* self, UIInput* input)
{
    assert(UIObject_isOfType(self, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)self;
    selfSelectable->m_onFocus(selfSelectable, input);
    if (selfSelectable->m_userOnFocus)
    {
        selfSelectable->m_userOnFocus(selfSelectable, input);
    }
}
