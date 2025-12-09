/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "ui/ui_input.h"
#include "ui/ui_utils.h"
#include "ui/ui_selectable.h"
#include "ui/ui_button.h"

void UIListVM_onRender(void* self);
void UIListVM_onDestroy(void* self);
void UIListVM_onUpdate(void* self);
void UIListVM_onFocusChanged(void* self, UIFocusState currState, UIFocusState prevState);
void UIListVM_onFocus(void* self, UIInput* input);
void UIListVM_onItemChanged(void* self, int currItemIdx, int prevItemIdx, bool increase);

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

    TTF_Text* m_itemText;
    TTF_Text* m_labelText;
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
    int m_spriteIndices[UI_BUTTON_STATE_COUNT];

    Vec2 m_itemAnchor;
    Vec2 m_labelAnchor;

    UITransform m_labelTransform;
    UITransform m_itemTransform;

    bool m_hasPrevItem;
    bool m_hasNextItem;
    bool m_useColorMod;
    UIButton* m_prevButton;
    UIButton* m_nextButton;

    void (*m_onItemChanged)(void* self, int currItemIdx, int prevItemIdx, bool increase);
    void (*m_userOnItemChanged)(void* self, int currItemIdx, int prevItemIdx, bool increase);
} UIList;

UIList* UIList_create(const char* objectName, TTF_Font* font, int itemCount, int configFlags);
void UIList_init(void* self, const char* objectName, TTF_Font* font, int itemCount, int configFlags);

INLINE void UIList_destroy(void* self)
{
    UIObject_destroy(self);
}

INLINE void UIList_setLabelString(void* self, const char* text)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    assert(text && "text must not be NULL");
    UIList* selfList = (UIList*)self;
    assert(selfList->m_labelString && "Existing label string must not be NULL");
    SDL_free(selfList->m_labelString);
    selfList->m_labelString = SDL_strdup(text);
}

INLINE void UIList_setItemString(void* self, int itemIdx, const char* text)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    assert(text && "text must not be NULL");
    UIList* selfList = (UIList*)self;
    assert(itemIdx >= 0 && itemIdx < selfList->m_itemCount && "itemIdx out of bounds");
    assert(selfList->m_itemStrings[itemIdx] && "Existing item string must not be NULL");
    SDL_free(selfList->m_itemStrings[itemIdx]);
    selfList->m_itemStrings[itemIdx] = SDL_strdup(text);
}

INLINE void UIList_setSelectedItem(void* self, int itemIdx)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    assert(itemIdx >= 0 && itemIdx < selfList->m_itemCount && "itemIdx out of bounds");
    selfList->m_itemIdx = itemIdx;
}

INLINE int UIList_getSelectedItem(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    return selfList->m_itemIdx;
}

INLINE void UIList_setOnItemChangedCallback(
    void* self,
    void (*onItemChanged)(void* self, int currItemIdx, int prevItemIdx, bool increase))
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_onItemChanged = onItemChanged;
}

INLINE UIButton* UIList_getPrevButton(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    return selfList->m_prevButton;
}

INLINE UIButton* UIList_getNextButton(void* self)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    return selfList->m_nextButton;
}

INLINE void UIList_setTextColor(void* self, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_itemColors[state] = color;
}

INLINE void UIList_setBackgroundColor(void* self, UIListState state, SDL_Color color)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_backColors[state] = color;
}

INLINE void UIList_setItemAnchor(void* self, Vec2 anchor)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_itemAnchor = anchor;
}

INLINE void UIList_setLabelAnchor(void* self, Vec2 anchor)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_labelAnchor = anchor;
}

INLINE void UIList_setSpriteGroup(void* self, SpriteGroup* spriteGroup)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_spriteGroup = spriteGroup;
}

INLINE void UIList_setSpriteIndex(void* self, UIListState state, int spriteIndex)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_spriteIndices[state] = spriteIndex;
}

INLINE void UIList_setUseColorMod(void* self, bool useColorMod)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_useColorMod = useColorMod;
}

INLINE void UIList_setLabelRect(void* self, UIRect rect)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_labelTransform.localRect = rect;
}

INLINE void UIList_setItemRect(void* self, UIRect rect)
{
    assert(UIObject_isOfType(self, UI_TYPE_LIST) && "self must be of type UI_TYPE_LIST");
    UIList* selfList = (UIList*)self;
    selfList->m_itemTransform.localRect = rect;
}
