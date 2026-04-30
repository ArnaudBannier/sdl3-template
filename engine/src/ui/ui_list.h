/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_selectable.h"
#include "ui/ui_button.h"

void UIListVM_onRender(void* selfPtr, GraphicsSystem* graphicsSystem);
void UIListVM_onDestroy(void* selfPtr);
void UIListVM_onUpdate(void* selfPtr);
void UIListVM_onFocusChanged(void* selfPtr, UIFocusState currState, UIFocusState prevState);
void UIListVM_onFocus(void* selfPtr, UIInput* input);
void UIListVM_onItemChanged(void* selfPtr, int currItemIdx, int prevItemIdx, bool increase);
#define UIListVM_onDrawGizmos UISelectableVM_onDrawGizmos

typedef enum UIListConfigFlags
{
    UI_LIST_CONFIG_CYCLE = 0x01,
    UI_LIST_CONFIG_AUTO_NAVIGATION = 0x02,
} UIListConfigFlags;


typedef enum UIListState
{
    UI_LIST_STATE_NORMAL = 0,
    UI_LIST_STATE_FOCUSED,
    UI_LIST_STATE_ACTIVE,
    UI_LIST_STATE_DISABLED,
    UI_LIST_STATE_COUNT
} UIListState;

typedef struct UIList
{
    UISelectable m_baseSelectable;

    UITransform m_labelTransform;
    UITransform m_itemTransform;

    RenderText m_labelText;
    RenderText m_itemText;

    int m_itemCount;
    int m_itemIdx;
    int m_configFlags;
    bool m_isActive;
    UIListState m_listState;

    char* m_labelString;
    char** m_itemStrings;
    SDL_Color m_itemColors[UI_LIST_STATE_COUNT];
    SDL_Color m_labelColors[UI_LIST_STATE_COUNT];
    SDL_Color m_backColors[UI_LIST_STATE_COUNT];

    SpriteGroup* m_spriteGroup;
    int m_spriteIndices[UI_LIST_STATE_COUNT];

    bool m_hasPrevItem;
    bool m_hasNextItem;
    bool m_useColorMod;
    UIButton* m_prevButton;
    UIButton* m_nextButton;

    void (*m_onItemChanged)(void* selfPtr, int currItemIdx, int prevItemIdx, bool increase);
    void (*m_userOnItemChanged)(void* selfPtr, int currItemIdx, int prevItemIdx, bool increase);
} UIList;

UIList* UIList_create(UISystem* uiSystem, const char* objectName, TTF_Font* font, int itemCount, int configFlags);
void UIList_init(void* selfPtr, UISystem* uiSystem, const char* objectName, TTF_Font* font, int itemCount, int configFlags);

INLINE void UIList_destroy(void* selfPtr)
{
    UIObject_destroy(selfPtr);
}

INLINE void UIList_setLabelString(void* selfPtr, const char* text)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    assert(text && "ttfText must not be NULL");
    UIList* selfList = (UIList*)selfPtr;
    assert(selfList->m_labelString && "Existing label string must not be NULL");
    SDL_free(selfList->m_labelString);
    selfList->m_labelString = SDL_strdup(text);
}

INLINE void UIList_setItemString(void* selfPtr, int itemIdx, const char* text)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    assert(text && "ttfText must not be NULL");
    UIList* selfList = (UIList*)selfPtr;
    assert(itemIdx >= 0 && itemIdx < selfList->m_itemCount && "itemIdx out of bounds");
    assert(selfList->m_itemStrings[itemIdx] && "Existing item string must not be NULL");
    SDL_free(selfList->m_itemStrings[itemIdx]);
    selfList->m_itemStrings[itemIdx] = SDL_strdup(text);
}

INLINE void UIList_setSelectedItem(void* selfPtr, int itemIdx)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    assert(itemIdx >= 0 && itemIdx < selfList->m_itemCount && "itemIdx out of bounds");
    selfList->m_itemIdx = itemIdx;
}

INLINE int UIList_getSelectedItem(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    return selfList->m_itemIdx;
}

INLINE void UIList_setOnItemChangedCallback(
    void* selfPtr,
    void (*onItemChanged)(void* selfPtr, int currItemIdx, int prevItemIdx, bool increase))
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_userOnItemChanged = onItemChanged;
}

INLINE UIButton* UIList_getPrevButton(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    return selfList->m_prevButton;
}

INLINE UIButton* UIList_getNextButton(void* selfPtr)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    return selfList->m_nextButton;
}

INLINE void UIList_setTextColor(void* selfPtr, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_itemColors[state] = color;
    selfList->m_labelColors[state] = color;
}

INLINE void UIList_setLabelColor(void* selfPtr, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_labelColors[state] = color;
}

INLINE void UIList_setItemColor(void* selfPtr, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_itemColors[state] = color;
}

INLINE void UIList_setBackgroundColor(void* selfPtr, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_backColors[state] = color;
}

INLINE void UIList_setItemAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_itemText.anchor = anchor;
}

INLINE void UIList_setLabelAnchor(void* selfPtr, Vec2 anchor)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_labelText.anchor = anchor;
}

INLINE void UIList_setSpriteGroup(void* selfPtr, SpriteGroup* spriteGroup)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_spriteGroup = spriteGroup;
}

INLINE void UIList_setSpriteIndex(void* selfPtr, UIListState state, int spriteIndex)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_spriteIndices[state] = spriteIndex;
}

INLINE void UIList_setUseColorMod(void* selfPtr, bool useColorMod)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_useColorMod = useColorMod;
}

INLINE void UIList_setLabelRect(void* selfPtr, UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_labelTransform.localRect = rect;
}

INLINE void UIList_setItemRect(void* selfPtr, UIRect rect)
{
    assert(UIObject_isOfType(selfPtr, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)selfPtr;
    selfList->m_itemTransform.localRect = rect;
}
