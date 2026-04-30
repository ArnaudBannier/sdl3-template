/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_object.h"
#include "core/sprite_sheet.h"
#include "core/graphics_system.h"
#include "utils/utils.h"

typedef enum UIFocusState
{
    UI_FOCUS_STATE_NORMAL = 0,
    UI_FOCUS_STATE_FOCUSED,
    UI_FOCUS_STATE_DISABLED,
    UI_FOCUS_STATE_COUNT
} UIFocusState;

void UISelectableVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState);
void UISelectableVM_onFocus(void* selfPtr, UIInput* input);
#define UISelectableVM_onDestroy UIObjectVM_onDestroy
#define UISelectableVM_onDrawGizmos UIObjectVM_onDrawGizmos
#define UISelectableVM_onUpdate UIObjectVM_onUpdate
#define UISelectableVM_onRender UIObjectVM_onRender

typedef struct UISelectable
{
    UIObject m_baseObject;

    UIFocusState m_focusState;
    int m_handledActionFlags;

    int m_audioOnFocused;
    int m_audioOnPressed;
    int m_audioOnClick;

    int m_userId;
    void* m_userData;

    void (*m_onFocusChanged)(void* selfPtr, UIFocusState currState, UIFocusState prevState);
    void (*m_userOnFocusChanged)(void* selfPtr, UIFocusState currState, UIFocusState prevState);

    void (*m_onFocus)(void* selfPtr, UIInput* input);
    void (*m_userOnFocus)(void* selfPtr, UIInput* input);

} UISelectable;

UISelectable* UISelectable_create(UISystem* uiSystem, const char* objectName);
void UISelectable_init(void* selfPtr, UISystem* uiSystem, const char* objectName);
INLINE void UISelectable_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UISelectable_setHandledActions(void* selfPtr, int actionFlags)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_handledActionFlags = actionFlags;
}

INLINE bool UISelectable_shouldHandleAction(void* selfPtr, int actionFlags)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return ((selfSelectable->m_handledActionFlags & actionFlags) != 0);
}

void UISelectable_setFocusState(void* selfPtr, UIFocusState state);

INLINE UIFocusState UISelectable_getFocusState(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_focusState;
}

INLINE void UISelectable_setUserData(void* selfPtr, void* userData)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_userData = userData;
}

INLINE void* UISelectable_getUserData(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_userData;
}

INLINE void UISelectable_setUserId(void* selfPtr, int userId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_userId = userId;
}

INLINE int UISelectable_getUserId(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_userId;
}

INLINE void UISelectable_setOnFocusChangedCallback(
    void* selfPtr,
    void (*onFocusChanged)(void* selfPtr, UIFocusState currState, UIFocusState prevState))
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_onFocusChanged = onFocusChanged;
}

INLINE void UISelectable_onFocus(void* selfPtr, UIInput* input)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_onFocus(selfSelectable, input);
    if (selfSelectable->m_userOnFocus)
    {
        selfSelectable->m_userOnFocus(selfSelectable, input);
    }
}

INLINE void UISelectable_setAudioOnFocused(void* selfPtr, int audioId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_audioOnFocused = audioId;
}

INLINE void UISelectable_setAudioOnPressed(void* selfPtr, int audioId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_audioOnPressed = audioId;
}

INLINE void UISelectable_setAudioOnClick(void* selfPtr, int audioId)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    selfSelectable->m_audioOnClick = audioId;
}

INLINE int UISelectable_getAudioOnFocused(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_audioOnFocused;
}

INLINE int UISelectable_getAudioOnPressed(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_audioOnPressed;
}

INLINE int UISelectable_getAudioOnClick(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    return selfSelectable->m_audioOnClick;
}
