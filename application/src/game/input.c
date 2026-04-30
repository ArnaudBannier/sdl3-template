/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/input.h"

#define TRIGGER_MAX_VALUE 32767

void PlayerInput_setTriggerL(PlayerInput *playerInput, Sint16 value)
{
    float trigger = Float_clamp((float)value / 32767.f, -1.f, 1.f);
    if (trigger < 0.3f) trigger = 0.f;
    playerInput->triggerL = trigger;
}

void PlayerInput_setTriggerR(PlayerInput *playerInput, Sint16 value)
{
    float trigger = Float_clamp((float)value / 32767.f, -1.f, 1.f);
    if (trigger < 0.3f) trigger = 0.f;
    playerInput->triggerR = trigger;
}

Input* Input_create(UISystem* uiSystem)
{
    Input* self = (Input*)calloc(1, sizeof(Input));
    ASSERT_NEW(self);

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

    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        AxisData_init(&(self->players[i].axisLeftData), 0.3f);
    }

    UIInput_init(&(self->uiInput), uiSystem);

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
            self->app.quitPressed = true;
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

void Input_clearUIInput(Input* self)
{
    UIInput* uiInput = &(self->uiInput);
    UIInput_beforeEventLoop(uiInput);
    Input_afterEventLoop(self);
}

void Input_beforeEventLoop(Input* self)
{
    DebugInput* debugInput = &(self->debug);
    MouseInput* mouseInput = &(self->mouse);
    ApplicationInput* appInput = &(self->app);

    appInput->pausePressed = false;
    appInput->quitPressed = false;

    mouseInput->wheel = 0;
    mouseInput->leftPressed = false;
    mouseInput->rightPressed = false;
    mouseInput->moved = false;

    debugInput->uiGizmosPressed = false;
    debugInput->objectGizmosPressed = false;
    debugInput->changeModePressed = false;
    debugInput->makeStepPressed = false;

    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        PlayerInput* playerInput = &(self->players[i]);
        AxisData_resetPressed(&(playerInput->axisLeftData));

        playerInput->shootPressed = false;
        playerInput->rocketPressed = false;
        playerInput->gunPressed = false;

        playerInput->validatePressed = false;
        playerInput->cancelPressed = false;
        playerInput->pausePressed = false;

        playerInput->upPressed = false;
        playerInput->downPressed = false;
        playerInput->leftPressed = false;
        playerInput->rightPressed = false;
    }
}

void Input_afterEventLoop(Input* self)
{
    MouseInput* mouseInput = &(self->mouse);
    ApplicationInput* appInput = &(self->app);
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

    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        PlayerInput* playerInput = &(self->players[i]);
        AxisData *axisData = &(playerInput->axisLeftData);

        playerInput->axis = AxisData_getAxis(axisData);
        playerInput->upPressed = AxisData_isUpPressed(axisData);
        playerInput->downPressed = AxisData_isDownPressed(axisData);
        playerInput->rightPressed = AxisData_isRightPressed(axisData);
        playerInput->leftPressed = AxisData_isLeftPressed(axisData);

        appInput->pausePressed = appInput->pausePressed || playerInput->pausePressed;
    }
}

void Input_processControllerButtonDown(Input* self, PlayerInput* playerInput, int button)
{
    AxisData *axisLeftData = &(playerInput->axisLeftData);
    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        playerInput->shootPressed = true;
        playerInput->shootDown = true;
        playerInput->validatePressed = true;
        break;
    case SDL_GAMEPAD_BUTTON_WEST:
        playerInput->cancelPressed = true;
        playerInput->gunPressed = true;
        break;
    case SDL_GAMEPAD_BUTTON_EAST:
        playerInput->rocketPressed = true;
        break;
    case SDL_GAMEPAD_BUTTON_START:
        playerInput->pausePressed = true;
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        AxisData_setDirectionUp(axisLeftData, true);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        AxisData_setDirectionDown(axisLeftData, true);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        AxisData_setDirectionLeft(axisLeftData, true);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        AxisData_setDirectionRight(axisLeftData, true);
        break;
    default: break;
    }
}

void Input_processControllerButtonUp(Input* self, PlayerInput* playerInput, int button)
{
    AxisData *axisLeftData = &(playerInput->axisLeftData);
    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        playerInput->shootDown = false;
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        AxisData_setDirectionUp(axisLeftData, false);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        AxisData_setDirectionDown(axisLeftData, false);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        AxisData_setDirectionLeft(axisLeftData, false);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        AxisData_setDirectionRight(axisLeftData, false);
        break;
    default: break;
    }
}

void Input_processControllerAxisMotion(Input* self, PlayerInput* playerInput, int axis, Sint16 value)
{
    AxisData *axisLeftData = &(playerInput->axisLeftData);
    switch (axis)
    {
    case SDL_GAMEPAD_AXIS_LEFTX:
        AxisData_setValueX(axisLeftData, value);
        break;

    case SDL_GAMEPAD_AXIS_LEFTY:
        AxisData_setValueY(axisLeftData, value);
        break;

    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        PlayerInput_setTriggerL(playerInput, value);
        break;

    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
        PlayerInput_setTriggerR(playerInput, value);
        break;

    default: break;
    }
}

void Input_processKeyDown(Input* self, PlayerInput* playerInput, bool repeat, int scancode)
{
    if (repeat) return;

    AxisData *axisLeftData = &(playerInput->axisLeftData);
    switch (scancode)
    {
    case SDL_SCANCODE_F1:
        self->debug.changeModePressed = true;
        break;
    case SDL_SCANCODE_F2:
        self->debug.objectGizmosPressed = true;
        break;
    case SDL_SCANCODE_F3:
        self->debug.uiGizmosPressed = true;
        break;
    case SDL_SCANCODE_TAB:
        self->debug.makeStepPressed = true;
        break;
    case SDL_SCANCODE_ESCAPE:
        self->app.quitPressed = true;
        break;
    case SDL_SCANCODE_UP:
        AxisData_setDirectionUp(axisLeftData, true);
        break;
    case SDL_SCANCODE_DOWN:
        AxisData_setDirectionDown(axisLeftData, true);
        break;
    case SDL_SCANCODE_LEFT:
        AxisData_setDirectionLeft(axisLeftData, true);
        break;
    case SDL_SCANCODE_RIGHT:
        AxisData_setDirectionRight(axisLeftData, true);
        break;
    case SDL_SCANCODE_LCTRL:
        PlayerInput_setTriggerL(playerInput, TRIGGER_MAX_VALUE);
        break;
    case SDL_SCANCODE_LALT:
        PlayerInput_setTriggerR(playerInput, TRIGGER_MAX_VALUE);
        break;
    case SDL_SCANCODE_SPACE:
        PlayerInput_setTriggerR(playerInput, TRIGGER_MAX_VALUE);
        playerInput->validatePressed = true;
        playerInput->shootPressed = true;
        playerInput->shootDown = true;
        break;
    case SDL_SCANCODE_Z:
        playerInput->rocketPressed = true;
        break;
    case SDL_SCANCODE_X:
        playerInput->gunPressed = true;
        break;
    case SDL_SCANCODE_RETURN:
        playerInput->pausePressed = true;
        break;
    case SDL_SCANCODE_BACKSPACE:
        playerInput->cancelPressed = true;
        break;
    default: break;
    }
}

void Input_processKeyUp(Input* self, PlayerInput* playerInput, bool repeat, int scancode)
{
    if (repeat) return;

    AxisData *axisLeftData = &(playerInput->axisLeftData);
    switch (scancode)
    {
    case SDL_SCANCODE_UP:
        AxisData_setDirectionUp(axisLeftData, false);
        break;
    case SDL_SCANCODE_DOWN:
        AxisData_setDirectionDown(axisLeftData, false);
        break;
    case SDL_SCANCODE_LEFT:
        AxisData_setDirectionLeft(axisLeftData, false);
        break;
    case SDL_SCANCODE_RIGHT:
        AxisData_setDirectionRight(axisLeftData, false);
        break;
    case SDL_SCANCODE_LCTRL:
        PlayerInput_setTriggerL(playerInput, 0);
        break;
    case SDL_SCANCODE_LALT:
        PlayerInput_setTriggerR(playerInput, 0);
        break;
    case SDL_SCANCODE_SPACE:
        PlayerInput_setTriggerR(playerInput, 0);
        playerInput->shootDown = false;
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
