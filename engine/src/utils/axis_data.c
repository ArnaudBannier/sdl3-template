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

void AxisData_init(AxisData *self, float deadZone)
{
    assert(self && "self must not be NULL");
    self->m_axis.x = 0.f;
    self->m_axis.y = 0.f;
    self->m_flags = 0;
    self->m_deadZoneSquared = deadZone * deadZone;
}

void AxisData_resetPressed(AxisData *self)
{
    assert(self && "self must not be NULL");
    self->m_flags &= ~AXIS_BUTTON_PRESSED;
}

Vec2 AxisData_getAxis(AxisData *self)
{
    assert(self && "self must not be NULL");
    float lengthSquared = Vec2_lengthSquared(self->m_axis);
    if (lengthSquared < self->m_deadZoneSquared)
    {
        return Vec2_zero;
    }
    if (lengthSquared > 1.f)
    {
        return Vec2_scale(self->m_axis, 1.f / sqrtf(lengthSquared));
    }

    return self->m_axis;
}

void AxisData_setValueX(AxisData *self, Sint16 value)
{
    assert(self && "self must not be NULL");
    if (self->m_flags & AXIS_BUTTON_DOWN)
        return;

    float deadZone = self->m_deadZoneSquared;
    float axis = Float_clamp((float)value / 32767.f, -1.f, 1.f);

    if (axis >= +0.8f && self->m_axis.x < +0.8f)
        self->m_flags |= AXIS_RIGHT_PRESSED;
    if (axis <= -0.8f && self->m_axis.x > -0.8f)
        self->m_flags |= AXIS_LEFT_PRESSED;

    self->m_axis.x = axis;
}

void AxisData_setValueY(AxisData *self, Sint16 value)
{
    assert(self && "self must not be NULL");
    if (self->m_flags & AXIS_BUTTON_DOWN)
        return;

    float deadZone = self->m_deadZoneSquared;
    float axis = -Float_clamp((float)value / 32767.f, -1.f, 1.f);

    if (axis >= +0.8f && self->m_axis.y < +0.8f)
        self->m_flags |= AXIS_UP_PRESSED;
    if (axis <= -0.8f && self->m_axis.y > -0.8f)
        self->m_flags |= AXIS_DOWN_PRESSED;

    self->m_axis.y = axis;
}

void AxisData_setDirectionUp(AxisData *self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->m_flags & AXIS_UP_DOWN))
        {
            self->m_flags |= AXIS_UP_PRESSED | AXIS_UP_DOWN;
        }
        self->m_axis.y = +1.f;
    }
    else
    {
        self->m_flags &= ~AXIS_UP_DOWN;
        if (self->m_axis.y >= +1.f)
            self->m_axis.y = 0.f;
    }
}

void AxisData_setDirectionDown(AxisData *self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->m_flags & AXIS_DOWN_DOWN))
        {
            self->m_flags |= AXIS_DOWN_PRESSED | AXIS_DOWN_DOWN;
        }
        self->m_axis.y = -1.f;
    }
    else
    {
        self->m_flags &= ~AXIS_DOWN_DOWN;
        if (self->m_axis.y <= -1.f)
            self->m_axis.y = 0.f;
    }
}

void AxisData_setDirectionRight(AxisData *self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->m_flags & AXIS_RIGHT_DOWN))
        {
            self->m_flags |= AXIS_RIGHT_PRESSED | AXIS_RIGHT_DOWN;
        }
        self->m_axis.x = +1.f;
    }
    else
    {
        self->m_flags &= ~AXIS_RIGHT_DOWN;
        if (self->m_axis.x >= +1.f)
            self->m_axis.x = 0.f;
    }
}

void AxisData_setDirectionLeft(AxisData *self, bool buttonDown)
{
    assert(self && "self must not be NULL");
    if (buttonDown)
    {
        if (!(self->m_flags & AXIS_LEFT_DOWN))
        {
            self->m_flags |= AXIS_LEFT_PRESSED | AXIS_LEFT_DOWN;
        }
        self->m_axis.x = -1.f;
    }
    else
    {
        self->m_flags &= ~AXIS_LEFT_DOWN;
        if (self->m_axis.x <= -1.f)
            self->m_axis.x = 0.f;
    }
}

bool AxisData_isUpPressed(AxisData *self)
{
    assert(self && "self must not be NULL");
    return (self->m_flags & AXIS_UP_PRESSED) == AXIS_UP_PRESSED;
}

bool AxisData_isDownPressed(AxisData *self)
{
    assert(self && "self must not be NULL");
    return (self->m_flags & AXIS_DOWN_PRESSED) == AXIS_DOWN_PRESSED;
}

bool AxisData_isLeftPressed(AxisData *self)
{
    assert(self && "self must not be NULL");
    return (self->m_flags & AXIS_LEFT_PRESSED) == AXIS_LEFT_PRESSED;
}

bool AxisData_isRightPressed(AxisData *self)
{
    assert(self && "self must not be NULL");
    return (self->m_flags & AXIS_RIGHT_PRESSED) == AXIS_RIGHT_PRESSED;
}
