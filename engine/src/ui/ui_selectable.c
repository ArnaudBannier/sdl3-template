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

    selfObject->type |= UI_TYPE_SELECTABLE;
    selfObject->gizmosColor = g_colors.grape5;
    selfObject->gizmosIsFilled = true;
    selfObject->gizmosOpacity = 0.25f;

    selfSelectable->handledActionFlags = 0;
    selfSelectable->focusState = UI_FOCUS_STATE_NORMAL;
    selfSelectable->audioOnClick = -1;
    selfSelectable->audioOnFocused = -1;
    selfSelectable->audioOnPressed = -1;

    // Virtual methods
    selfObject->onRender = UISelectableVM_onRender;
    selfObject->onUpdate = UISelectableVM_onUpdate;
    selfObject->onDestroy = UISelectableVM_onDestroy;
    selfObject->onDrawGizmos = UISelectableVM_onDrawGizmos;
    selfSelectable->onFocus = UISelectableVM_onFocus;
    selfSelectable->onFocusChanged = UISelectableVM_onFocusChanged;

    selfSelectable->userOnFocus = NULL;
    selfSelectable->userOnFocusChanged = NULL;
}

void UISelectable_setFocusState(void* selfPtr, UIFocusState state)
{
    UISelectable* selfSelectable = (UISelectable*)selfPtr;
    assert(UIObject_isOfType(selfPtr, UI_TYPE_SELECTABLE) && "self must be of type UI_TYPE_SELECTABLE");

    if (selfSelectable->focusState == state) return;

    int prevState = selfSelectable->focusState;
    selfSelectable->focusState = state;

    selfSelectable->onFocusChanged(selfSelectable, state, prevState);
    if (selfSelectable->userOnFocusChanged)
    {
        selfSelectable->userOnFocusChanged(selfSelectable, state, prevState);
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
