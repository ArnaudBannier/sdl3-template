/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/input.h"

#define TRIGGER_MAX_VALUE 32767

Input* Input_create()
{
    Input* self = (Input*)calloc(1, sizeof(Input));
    AssertNew(self);

    int playerID = 0;
    int gamepadCount = 0;
    SDL_JoystickID* joysticks = SDL_GetGamepads(&gamepadCount);
    if (joysticks)
    {
        for (int i = 0; i < gamepadCount; i++)
        {
            SDL_Gamepad* gamepad = SDL_OpenGamepad(joysticks[i]);
            SDL_SetGamepadPlayerIndex(gamepad, playerID);
            playerID += (playerID < MAX_PLAYER_COUNT - 1) ? 1 : 0;
        }
        SDL_free(joysticks);
    }

    UIInput_init(&(self->uiInput));

    return self;
}

void Input_destroy(Input* self)
{
    if (!self) return;
    free(self);
}

void Input_update(Input* self)
{
    assert(self);
    PlayerInput* playerInput = NULL;
    UIInput* uiInput = &(self->uiInput);
    int playerID = 0;

    Input_beforeEventLoop(self);
    UIInput_beforeEventLoop(uiInput);

    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event))
    {
        UIInput_processEvent(uiInput, &event);
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            self->quitPressed = true;
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            playerID = SDL_GetGamepadPlayerIndexForID(event.gbutton.which);
            assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);
            playerInput = &(self->players[playerID]);
            Input_processControllerButtonDown(self, playerInput, event.gbutton.button);
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            playerID = SDL_GetGamepadPlayerIndexForID(event.gbutton.which);
            assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);
            playerInput = &(self->players[playerID]);
            Input_processControllerButtonUp(self, playerInput, event.gbutton.button);
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            playerID = SDL_GetGamepadPlayerIndexForID(event.gbutton.which);
            assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);
            playerInput = &(self->players[playerID]);
            Input_processControllerAxisMotion(self, playerInput, event.gaxis.axis, event.gaxis.value);
            break;

        case SDL_EVENT_KEY_DOWN:
            playerInput = &(self->players[0]);
            Input_processKeyDown(self, playerInput, event.key.repeat, event.key.scancode);
            break;

        case SDL_EVENT_KEY_UP:
            playerInput = &(self->players[0]);
            Input_processKeyUp(self, playerInput, event.key.repeat, event.key.scancode);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            Input_processMouseButtonDown(self, event.button.button);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            Input_processMouseButtonUp(self, event.button.button);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            Input_processMouseWheel(self, event.wheel.y);
            break;
        }
    }

    UIInput_afterEventLoop(uiInput);
    Input_afterEventLoop(self);
}

void Input_beforeEventLoop(Input* self)
{
    DebugInput* debugInput = &(self->debug);
    MouseInput* mouseInput = &(self->mouse);

    self->quitPressed = false;

    mouseInput->wheel = 0;
    mouseInput->leftPressed = false;
    mouseInput->rightPressed = false;
    mouseInput->moved = false;

    debugInput->gizmosPressed = false;

    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        PlayerInput* playerInput = &(self->players[i]);
        playerInput->validatePressed = false;
    }
}

void Input_afterEventLoop(Input* self)
{
    MouseInput* mouseInput = &(self->mouse);
    float windowX = 0;
    float windowY = 0;
    float mouseX = 0;
    float mouseY = 0;

    (void)SDL_GetMouseState(&mouseX, &mouseY);
    if (fabsf(mouseInput->position.x - mouseX) > 0.01f ||
        fabsf(mouseInput->position.y - mouseY) > 0.01f)
    {
        mouseInput->moved = true;
    }

    mouseInput->position.x = mouseX;
    mouseInput->position.y = mouseY;
}

void Input_processControllerButtonDown(Input* self, PlayerInput* playerInput, int button)
{
    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        playerInput->validatePressed = true;
        break;
    case SDL_GAMEPAD_BUTTON_WEST:
        break;
    case SDL_GAMEPAD_BUTTON_EAST:
        break;
    case SDL_GAMEPAD_BUTTON_START:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        break;
    default: break;
    }
}

void Input_processControllerButtonUp(Input* self, PlayerInput* playerInput, int button)
{
    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        break;
    default: break;
    }
}

void Input_processControllerAxisMotion(Input* self, PlayerInput* playerInput, int axis, Sint16 value)
{
    switch (axis)
    {
    case SDL_GAMEPAD_AXIS_LEFTX:
        break;

    case SDL_GAMEPAD_AXIS_LEFTY:
        break;

    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        break;

    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
        break;

    default: break;
    }
}

void Input_processKeyDown(Input* self, PlayerInput* playerInput, bool repeat, int scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_F1:
        self->debug.gizmosPressed = true;
        break;
    case SDL_SCANCODE_ESCAPE:
        self->quitPressed = true;
        break;
    case SDL_SCANCODE_UP:
        break;
    case SDL_SCANCODE_DOWN:
        break;
    case SDL_SCANCODE_LEFT:
        break;
    case SDL_SCANCODE_RIGHT:
        break;
    case SDL_SCANCODE_SPACE:
        playerInput->validatePressed = true;
        break;
    case SDL_SCANCODE_RETURN:
        break;
    case SDL_SCANCODE_BACKSPACE:
        break;
    default: break;
    }
}

void Input_processKeyUp(Input* self, PlayerInput* playerInput, bool repeat, int scancode)
{
    if (repeat) return;

    switch (scancode)
    {
    case SDL_SCANCODE_SPACE:
        break;
    default: break;
    }
}

void Input_processMouseButtonDown(Input* self, int button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT:
        self->mouse.leftPressed = true;
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    case SDL_BUTTON_RIGHT:
        self->mouse.rightPressed = true;
        break;
    default: break;
    }
}

void Input_processMouseButtonUp(Input* self, int button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT:
        break;
    case SDL_BUTTON_MIDDLE:
        break;
    case SDL_BUTTON_RIGHT:
        break;
    default: break;
    }
}

void Input_processMouseWheel(Input* self, float wheel)
{
    self->mouse.wheel += (int)wheel;
}
