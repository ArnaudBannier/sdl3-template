/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "engine_settings.h"
#include "utils/utils.h"

typedef struct AxisData
{
    Vec2 m_axis;
    float m_deadZoneSquared;
    int m_flags;
} AxisData;

void AxisData_init(AxisData *self, float deadZone);
void AxisData_resetPressed(AxisData *self);
Vec2 AxisData_getAxis(AxisData *self);
void AxisData_setValueX(AxisData *self, Sint16 value);
void AxisData_setValueY(AxisData *self, Sint16 value);

void AxisData_setDirectionUp(AxisData *self, bool buttonDown);
void AxisData_setDirectionDown(AxisData *self, bool buttonDown);
void AxisData_setDirectionLeft(AxisData *self, bool buttonDown);
void AxisData_setDirectionRight(AxisData *self, bool buttonDown);

bool AxisData_isUpPressed(AxisData *self);
bool AxisData_isDownPressed(AxisData *self);
bool AxisData_isLeftPressed(AxisData *self);
bool AxisData_isRightPressed(AxisData *self);
