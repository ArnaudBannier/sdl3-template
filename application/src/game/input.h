/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"

typedef struct PlayerInput
{
    Vec2 axis;

    float triggerL;
    float triggerR;

    bool shootDown;
    bool boostDown;
    bool shootPressed;
    bool rocketPressed;
    bool gunPressed;

    bool validatePressed;
    bool cancelPressed;
    bool pausePressed;

    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    AxisData axisLeftData;
} PlayerInput;

void PlayerInput_setTriggerL(PlayerInput* playerInput, Sint16 value);
void PlayerInput_setTriggerR(PlayerInput* playerInput, Sint16 value);

typedef struct DebugInput
{
    bool uiGizmosPressed;
    bool objectGizmosPressed;
    bool makeStepPressed;
    bool changeModePressed;
} DebugInput;

typedef struct MouseInput
{
    bool leftPressed;
    bool rightPressed;
    bool moved;
    int wheel;
    Vec2 position;
} MouseInput;

typedef struct ApplicationInput
{
    bool pausePressed;
    bool quitPressed;
} ApplicationInput;

/// @brief Structure représentant le gestionnaire des entrées utilisateur.
typedef struct Input
{
    DebugInput debug;
    MouseInput mouse;
    PlayerInput players[MAX_PLAYER_COUNT];
    ApplicationInput app;
    UIInput uiInput;
} Input;

/// @brief Crée un nouveau gestionnaire des entrées utilisateur.
/// @return Le gestionnaire créé.
Input* Input_create(UISystem* uiSystem);

/// @brief Détruit le gestionnaire des entrées utilisateur.
/// @param self le gestionnaire.
void Input_destroy(Input* self);

/// @brief Met à jour le gestionnaire des entrées utilisateur.
/// Cette fonction effectue la boucle des événements SDL.
/// @param self le gestionnaire.
void Input_update(Input* self);

void Input_clearUIInput(Input* self);

void Input_beforeEventLoop(Input* self);
void Input_afterEventLoop(Input* self);

void Input_processControllerButtonDown(Input* self, PlayerInput* playerInput, int button);
void Input_processControllerButtonUp(Input* self, PlayerInput* playerInput, int button);
void Input_processControllerAxisMotion(Input* self, PlayerInput* playerInput, int axis, Sint16 value);
void Input_processKeyDown(Input* self, PlayerInput* playerInput, bool repeat, int scancode);
void Input_processKeyUp(Input* self, PlayerInput* playerInput, bool repeat, int scancode);
void Input_processMouseButtonDown(Input* self, int button);
void Input_processMouseButtonUp(Input* self, int button);
void Input_processMouseWheel(Input* self, float wheel);
