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
    bool validatePressed;
} PlayerInput;

typedef struct DebugInput
{
    bool gizmosPressed;
} DebugInput;

typedef struct MouseInput
{
    bool leftPressed;
    bool rightPressed;
    bool moved;
    int wheel;
    Vec2 position;
} MouseInput;

/// @brief Structure représentant le gestionnaire des entrées utilisateur.
typedef struct Input
{
    /// @brief Booléen indiquant si le bouton "quitter" vient d'être pressé.
    bool quitPressed;

    DebugInput debug;
    MouseInput mouse;
    PlayerInput players[MAX_PLAYER_COUNT];
    UIInput uiInput;
} Input;

/// @brief Crée un nouveau gestionnaire des entrées utilisateur.
/// @return Le gestionnaire créé.
Input* Input_create();

/// @brief Détruit le gestionnaire des entrées utilisateur.
/// @param self le gestionnaire.
void Input_destroy(Input* self);

/// @brief Met à jour le gestionnaire des entrées utilisateur.
/// Cette fonction effectue la boucle des événements SDL.
/// @param self le gestionnaire.
void Input_update(Input* self);

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
