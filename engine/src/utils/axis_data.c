/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "utils/axis_data.h"

typedef enum AxisDataFlags
{
    AXIS_UP_DOWN = 0x01,
    AXIS_DOWN_DOWN = 0x02,
    AXIS_LEFT_DOWN = 0x04,
    AXIS_RIGHT_DOWN = 0x08,
    AXIS_BUTTON_DOWN = 0x0F,
    AXIS_UP_PRESSED = 0x10,
    AXIS_DOWN_PRESSED = 0x20,
    AXIS_LEFT_PRESSED = 0x40,
    AXIS_RIGHT_PRESSED = 0x80,
    AXIS_BUTTON_PRESSED = 0xF0
} AxisDataFlags;

void AxisData_init(AxisData* self, float deadZone)
{
    assert(self && "self must not be NULL");
    self->axis.x = 0.f;
    self->axis.y = 0.f;
    self->flags = 0;
    self->deadZoneSquared = deadZone * deadZone;
}

void AxisData_resetPressed(AxisData* self)
{
    assert(self && "self must not be NULL");
    self->flags &= ~AXIS_BUTTON_PRESSED;
}

Vec2 AxisData_getAxis(AxisData* self)
{
    assert(self && "self must not be NULL");
    float lengthSquared = Vec2_lengthSquared(self->axis);
    if (lengthSquared < self->deadZoneSquared)
    {
        return Vec2_zero;
    }
    if (lengthSquared > 1.f)
    {
        return Vec2_scale(self->axis, 1.f / sqrtf(lengthSquared));
    }

    return self->axis;
}

void AxisData_setValueX(AxisData* self, Sint16 value)
{
    assert(self && "self must not be NULL");
    if (self->flags & AXIS_BUTTON_DOWN)
        return;

    float deadZone = self->deadZoneSquared;
    float axis = Float_clamp((float)value / 32767.f, -1.f, 1.f);

    if (axis >= +0.8f && self->axis.x < +0.8f)
        self->flags |= AXIS_RIGHT_PRESSED;
    if (axis <= -0.8f && self->axis.x > -0.8f)
        self->flags |= AXIS_LEFT_PRESSED;

    self->axis.x = axis;
}

void AxisData_setValueY(AxisData* self, Sint16 value)
{
    assert(self && "self must not be NULL");
    if (self->flags & AXIS_BUTTON_DOWN)
        return;

    float deadZone = self->deadZoneSquared;
    float axis = -Float_clamp((float)value / 32767.f, -1.f, 1.f);

    if (axis >= +0.8f && self->axis.y < +0.8f)
        self->flags |= AXIS_UP_PRESSED;
    if (axis <= -0.8f && self->axis.y > -0.8f)
        self->flags |= AXIS_DOWN_PRESSED;

    self->axis.y = axis;
}

void AxisData_setDirectionUp(AxisData* self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->flags & AXIS_UP_DOWN))
        {
            self->flags |= AXIS_UP_PRESSED | AXIS_UP_DOWN;
        }
        self->axis.y = +1.f;
    }
    else
    {
        self->flags &= ~AXIS_UP_DOWN;
        if (self->axis.y >= +1.f)
            self->axis.y = 0.f;
    }
}

void AxisData_setDirectionDown(AxisData* self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->flags & AXIS_DOWN_DOWN))
        {
            self->flags |= AXIS_DOWN_PRESSED | AXIS_DOWN_DOWN;
        }
        self->axis.y = -1.f;
    }
    else
    {
        self->flags &= ~AXIS_DOWN_DOWN;
        if (self->axis.y <= -1.f)
            self->axis.y = 0.f;
    }
}

void AxisData_setDirectionRight(AxisData* self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->flags & AXIS_RIGHT_DOWN))
        {
            self->flags |= AXIS_RIGHT_PRESSED | AXIS_RIGHT_DOWN;
        }
        self->axis.x = +1.f;
    }
    else
    {
        self->flags &= ~AXIS_RIGHT_DOWN;
        if (self->axis.x >= +1.f)
            self->axis.x = 0.f;
    }
}

void AxisData_setDirectionLeft(AxisData* self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->flags & AXIS_LEFT_DOWN))
        {
            self->flags |= AXIS_LEFT_PRESSED | AXIS_LEFT_DOWN;
        }
        self->axis.x = -1.f;
    }
    else
    {
        self->flags &= ~AXIS_LEFT_DOWN;
        if (self->axis.x <= -1.f)
            self->axis.x = 0.f;
    }
}

bool AxisData_isUpPressed(AxisData* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & AXIS_UP_PRESSED) == AXIS_UP_PRESSED;
}

bool AxisData_isDownPressed(AxisData* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & AXIS_DOWN_PRESSED) == AXIS_DOWN_PRESSED;
}

bool AxisData_isLeftPressed(AxisData* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & AXIS_LEFT_PRESSED) == AXIS_LEFT_PRESSED;
}

bool AxisData_isRightPressed(AxisData* self)
{
    assert(self && "self must not be NULL");
    return (self->flags & AXIS_RIGHT_PRESSED) == AXIS_RIGHT_PRESSED;
}
