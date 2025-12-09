/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"
#include "utils/utils.h"

typedef enum UIActionFlags
{
    UI_ACTION_UP = 0x01,
    UI_ACTION_DOWN = 0x02,
    UI_ACTION_LEFT = 0x04,
    UI_ACTION_RIGHT = 0x08,
    UI_ACTION_VALIDATE = 0x10,
    UI_ACTION_CLICK = 0x20,
    UI_ACTION_CANCEL = 0x40,
    UI_ACTION_MOUSE_MOVE = 0x80,
} UIActionFlags;

typedef enum UIInputType
{
    UI_INPUT_TYPE_KEYBOARD,
    UI_INPUT_TYPE_CONTROLLER,
    UI_INPUT_TYPE_MOUSE,
} UIInputType;

typedef struct UIInputKeyboardConfig
{
    SDL_Scancode upKey;
    SDL_Scancode downKey;
    SDL_Scancode leftKey;
    SDL_Scancode rightKey;
    SDL_Scancode validateKey;
    SDL_Scancode cancelKey;
} UIInputKeyboardConfig;

typedef struct UIInputControllerConfig
{
    int upButton;
    int downButton;
    int leftButton;
    int rightButton;
    int validateButton;
    int cancelButton;
    int axisX;
    int axisY;
} UIInputControllerConfig;

typedef struct UIInputConfig
{
    UIInputKeyboardConfig keyboardA;
    UIInputKeyboardConfig keyboardB;
    UIInputControllerConfig controller;
} UIInputConfig;

typedef struct UIInputData
{
    Sint16 m_activeZone;
    Sint16 m_axisX;
    Sint16 m_axisY;
} UIInputData;

typedef struct UIInput
{
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
    bool validatePressed;
    bool validateDown;
    bool validateReleased;
    bool clickPressed;
    bool clickDown;
    bool clickReleased;
    bool cancelPressed;
    bool mouseMoved;
    Vec2 mousePxPos;
    Vec2 mouseUIPos;
    Vec2 navDirection;
    UIInputType lastInputType;
    UIInputConfig config;
    UIInputData m_data;
} UIInput;

void UIInput_init(UIInput* self);
void UIInput_beforeEventLoop(UIInput* self);
void UIInput_processEvent(UIInput* self, SDL_Event* event);
void UIInput_afterEventLoop(UIInput* self);

