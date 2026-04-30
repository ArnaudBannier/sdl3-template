/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_selectable.h"
#include "engine_common.h"

UISelectable* UISelectable_create(UISystem* uiSystem, const char* objectName)
{
    UISelectable* self = (UISelectable*)calloc(1, sizeof(UISelectable));
    ASSERT_NEW(self);

    UISelectable_init(self, uiSystem, objectName);

    return self;
}

void UISelectable_init(void* selfPtr, UISystem* uiSystem, const char* objectName)
{
    assert(selfPtr && "self must not be NULL");

    UIObject* selfObject = (UIObject*)selfPtr;
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    UIObject_init(selfObject, uiSystem, objectName);

    selfObject->m_type |= UI_TYPE_SELECTABLE;
    selfObject->m_gizmosColor = g_colors.grape5;
    selfObject->m_gizmosIsFilled = true;
    selfObject->m_gizmosOpacity = 0.25f;

    selfSelectable->m_handledActionFlags = 0;
    selfSelectable->m_focusState = UI_FOCUS_STATE_NORMAL;
    selfSelectable->m_audioOnClick = -1;
    selfSelectable->m_audioOnFocused = -1;
    selfSelectable->m_audioOnPressed = -1;

    // Virtual methods
    selfObject->m_onRender = UISelectableVM_onRender;
    selfObject->m_onUpdate = UISelectableVM_onUpdate;
    selfObject->m_onDestroy = UISelectableVM_onDestroy;
    selfObject->m_onDrawGizmos = UISelectableVM_onDrawGizmos;
    selfSelectable->m_onFocus = UISelectableVM_onFocus;
    selfSelectable->m_onFocusChanged = UISelectableVM_onFocusChanged;

    selfSelectable->m_userOnFocus = NULL;
    selfSelectable->m_userOnFocusChanged = NULL;
}

void UISelectable_setFocusState(void* selfPtr, UIFocusState state)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");

    if (selfSelectable->m_focusState == state) return;

    int prevState = selfSelectable->m_focusState;
    selfSelectable->m_focusState = state;

    selfSelectable->m_onFocusChanged(selfSelectable, state, prevState);
    if (selfSelectable->m_userOnFocusChanged)
    {
        selfSelectable->m_userOnFocusChanged(selfSelectable, state, prevState);
    }
}

void UISelectableVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState)
{
    // Nothing to do
}

void UISelectableVM_onFocus(void* selfPtr, UIInput* input)
{
    // Nothing to do
}
