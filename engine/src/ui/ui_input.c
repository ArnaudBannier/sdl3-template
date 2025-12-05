/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ui/ui_input.h"
#include "game_engine_common.h"

void UIInput_init(UIInput* self)
{
    assert(self && "self must not be NULL");
    self->upPressed = false;
    self->downPressed = false;
    self->leftPressed = false;
    self->rightPressed = false;
    self->validatePressed = false;
    self->validateDown = false;
    self->validateReleased = false;
    self->clickPressed = false;
    self->clickDown = false;
    self->clickReleased = false;
    self->cancelPressed = false;
    self->mouseMoved = false;

    self->mousePxPos = Vec2_zero;
    self->mouseUIPos = Vec2_set(0, g_sizes.uiSize.y);
    self->navDirection = Vec2_zero;

    // Default keyboard config A
    self->config.keyboardA.upKey = SDL_SCANCODE_W;
    self->config.keyboardA.downKey = SDL_SCANCODE_S;
    self->config.keyboardA.leftKey = SDL_SCANCODE_A;
    self->config.keyboardA.rightKey = SDL_SCANCODE_D;
    self->config.keyboardA.validateKey = SDL_SCANCODE_SPACE;
    self->config.keyboardA.cancelKey = SDL_SCANCODE_BACKSPACE;
    // Default keyboard config B
    self->config.keyboardB.upKey = SDL_SCANCODE_UP;
    self->config.keyboardB.downKey = SDL_SCANCODE_DOWN;
    self->config.keyboardB.leftKey = SDL_SCANCODE_LEFT;
    self->config.keyboardB.rightKey = SDL_SCANCODE_RIGHT;
    self->config.keyboardB.validateKey = SDL_SCANCODE_RETURN;
    self->config.keyboardB.cancelKey = SDL_SCANCODE_ESCAPE;
    // Default controller config
    self->config.controller.upButton = SDL_GAMEPAD_BUTTON_DPAD_UP;
    self->config.controller.downButton = SDL_GAMEPAD_BUTTON_DPAD_DOWN;
    self->config.controller.leftButton = SDL_GAMEPAD_BUTTON_DPAD_LEFT;
    self->config.controller.rightButton = SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
    self->config.controller.validateButton = SDL_GAMEPAD_BUTTON_SOUTH;
    self->config.controller.cancelButton = SDL_GAMEPAD_BUTTON_EAST;
    self->config.controller.axisX = SDL_GAMEPAD_AXIS_LEFTX;
    self->config.controller.axisY = SDL_GAMEPAD_AXIS_LEFTY;

    // Internal data
    self->m_data.m_activeZone = 1 << 14;
    self->m_data.m_axisX = 0;
    self->m_data.m_axisY = 0;
}

void UIInput_beforeEventLoop(UIInput* self)
{
    assert(self && "self must not be NULL");
    self->upPressed = false;
    self->downPressed = false;
    self->leftPressed = false;
    self->rightPressed = false;
    self->validatePressed = false;
    self->validateReleased = false;
    self->clickPressed = false;
    self->clickReleased = false;
    self->cancelPressed = false;
    self->mouseMoved = false;
}

static void UIInput_processControllerButtonDown(UIInput* self, int button)
{
    self->lastInputType = UI_INPUT_TYPE_CONTROLLER;

    UIInputControllerConfig* config = &(self->config.controller);
    if (button == config->upButton)
    {
        self->upPressed = true;
    }
    else if (button == config->downButton)
    {
        self->downPressed = true;
    }
    else if (button == config->leftButton)
    {
        self->leftPressed = true;
    }
    else if (button == config->rightButton)
    {
        self->rightPressed = true;
    }
    else if (button == config->validateButton)
    {
        self->validatePressed = true;
        self->validateDown = true;
    }
    else if (button == config->cancelButton)
    {
        self->cancelPressed = true;
    }
}

static void UIInput_processControllerButtonUp(UIInput* self, int button)
{
    self->lastInputType = UI_INPUT_TYPE_CONTROLLER;

    UIInputControllerConfig* config = &(self->config.controller);
    if (button == config->validateButton)
    {
        self->validateReleased = true;
        self->validateDown = false;
    }
}

static void UIInput_processControllerAxisMotion(UIInput* self, int axis, Sint16 value)
{
    self->lastInputType = UI_INPUT_TYPE_CONTROLLER;

    UIInputControllerConfig* config = &(self->config.controller);
    UIInputData* data = &(self->m_data);
    if (axis == config->axisX)
    {
        if (value >= data->m_activeZone && data->m_axisX < data->m_activeZone)
        {
            self->rightPressed = true;
        }
        else if (value <= -data->m_activeZone && data->m_axisX > -data->m_activeZone)
        {
            self->leftPressed = true;
        }
        data->m_axisX = value;
    }
    else if (axis == config->axisY)
    {
        if (value >= data->m_activeZone && data->m_axisY < data->m_activeZone)
        {
            self->downPressed = true;
        }
        else if (value <= -data->m_activeZone && data->m_axisY > -data->m_activeZone)
        {
            self->upPressed = true;
        }
        data->m_axisY = value;
    }
}

static void UIInput_processKeyDown(UIInput* self, bool repeat, int scancode)
{
    self->lastInputType = UI_INPUT_TYPE_KEYBOARD;
    UIInputKeyboardConfig* configA = &(self->config.keyboardA);
    UIInputKeyboardConfig* configB = &(self->config.keyboardB);
    if (scancode == configA->upKey || scancode == configB->upKey)
    {
        self->upPressed = true;
    }
    else if (scancode == configA->downKey || scancode == configB->downKey)
    {
        self->downPressed = true;
    }
    else if (scancode == configA->leftKey || scancode == configB->leftKey)
    {
        self->leftPressed = true;
    }
    else if (scancode == configA->rightKey || scancode == configB->rightKey)
    {
        self->rightPressed = true;
    }
    else if (scancode == configA->validateKey || scancode == configB->validateKey)
    {
        if (repeat) return;
        self->validatePressed = true;
        self->validateDown = true;
    }
    else if (scancode == configA->cancelKey || scancode == configB->cancelKey)
    {
        if (repeat) return;
        self->cancelPressed = true;
    }
}

static void UIInput_processKeyUp(UIInput* self, bool repeat, int scancode)
{
    self->lastInputType = UI_INPUT_TYPE_KEYBOARD;
    if (repeat) return;

    UIInputKeyboardConfig* configA = &(self->config.keyboardA);
    UIInputKeyboardConfig* configB = &(self->config.keyboardB);
    if (scancode == configA->validateKey || scancode == configB->validateKey)
    {
        self->validateReleased = true;
        self->validateDown = false;
    }
}

static void UIInput_processMouseButtonDown(UIInput* self, int button)
{
    self->lastInputType = UI_INPUT_TYPE_MOUSE;
    if (button == SDL_BUTTON_LEFT)
    {
        self->clickDown = true;
        self->clickPressed = true;
    }
}

static void UIInput_processMouseButtonUp(UIInput* self, int button)
{
    self->lastInputType = UI_INPUT_TYPE_MOUSE;
    if (button == SDL_BUTTON_LEFT)
    {
        self->clickReleased = true;
        self->clickDown = false;
    }
}

void UIInput_processEvent(UIInput* self, SDL_Event* event)
{
    assert(self && "self must not be NULL");
    assert(event && "event must not be NULL");

    switch (event->type)
    {
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        UIInput_processControllerButtonDown(self, event->gbutton.button);
        break;

    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        UIInput_processControllerButtonUp(self, event->gbutton.button);
        break;

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        UIInput_processControllerAxisMotion(self, event->gaxis.axis, event->gaxis.value);
        break;

    case SDL_EVENT_KEY_DOWN:
        UIInput_processKeyDown(self, event->key.repeat, event->key.scancode);
        break;

    case SDL_EVENT_KEY_UP:
        UIInput_processKeyUp(self, event->key.repeat, event->key.scancode);
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        UIInput_processMouseButtonDown(self, event->button.button);
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        UIInput_processMouseButtonUp(self, event->button.button);
        break;

    default:
        break;
    }
}

void UIInput_afterEventLoop(UIInput* self)
{
    assert(self && "self must not be NULL");

    float windowX = 0;
    float windowY = 0;
    float mouseX = 0;
    float mouseY = 0;
    (void)SDL_GetMouseState(&windowX, &windowY);
    bool success = SDL_RenderCoordinatesFromWindow(g_renderer, windowX, windowY, &mouseX, &mouseY);
    if (!success)
    {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to compute coordinates");
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
        assert(false);
    }

    if (fabsf(self->mousePxPos.x - mouseX) < 0.01f &&
        fabsf(self->mousePxPos.y - mouseY) < 0.01f)
    {
        return;
    }

    self->mouseMoved = true;
    self->lastInputType = UI_INPUT_TYPE_MOUSE;

    self->mousePxPos.x = mouseX;
    self->mousePxPos.y = mouseY;
    self->mouseUIPos.x = mouseX / g_sizes.uiPixelsPerUnit.x;
    self->mouseUIPos.y = (g_sizes.viewportSize.y - mouseY) / g_sizes.uiPixelsPerUnit.y;
}
