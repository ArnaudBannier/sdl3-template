/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"

#define MAX_ANIM_EVENT_COUNT 8

typedef enum AnimFlag
{
    ANIM_FLAG_NONE = 0,
    /// @brief Met l'animation en pause.
    ANIM_FLAG_PAUSED = 1 << 0,
    /// @brief Joue l'animation dans le sens inverse.
    ANIM_FLAG_REVERSED = 1 << 1,
    /// @brief Joue l'animation en avant puis en arrière.
    ANIM_FLAG_ALTERNATE = 1 << 2,
    /// @brief Arrête automatiquement l'animation à la fin.
    ANIM_FLAG_STOP_AT_END = 1 << 3,
    ANIM_FLAG_STOPPED = 1 << 4
} AnimFlag;

typedef enum AnimEventType
{
    ANIM_EVENT_UNDEFINED = 0,
    ANIM_EVENT_ANIMATION_START,
    ANIM_EVENT_ANIMATION_END,
    ANIM_EVENT_CYCLE_END,
    ANIM_EVENT_FRAME_CHANGED
} AnimEventType;

typedef uint64_t AnimId;
