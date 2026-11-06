/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/camera.h"

Camera* Camera_create(Viewport* viewport)
{
    Camera* self = (Camera*)calloc(1, sizeof(Camera));
    ASSERT_NEW(self);

    const float worldW = 16.0f;
    const float worldH = 9.0f;

    self->worldView = AABB_set(0.0f, 0.0f, worldW, worldH);
    self->viewport = viewport;

    return self;
}

void Camera_destroy(Camera* self)
{
    if (!self) return;
    free(self);
}

void Camera_update(Camera* self)
{
    const float w = self->worldView.upper.x - self->worldView.lower.x;
    const float h = self->worldView.upper.y - self->worldView.lower.y;
    const float viewportW = Viewport_getWidth(self->viewport);
    const float viewportH = Viewport_getHeight(self->viewport);
    self->scale.x = viewportW / w;
    self->scale.y = viewportH / h;
}

float Camera_worldToViewX(const Camera* self, float positionX)
{
    assert(self && "self must not be NULL");
    return (positionX - self->worldView.lower.x) * self->scale.x;
}

float Camera_worldToViewY(const Camera* self, float positionY)
{
    assert(self && "self must not be NULL");
    return (self->worldView.upper.y - positionY) * self->scale.y;
}

Vec2 Camera_worldToViewV(const Camera* self, Vec2 vec)
{
    return Vec2_set(
        Camera_worldToViewX(self, vec.x),
        Camera_worldToViewY(self, vec.y)
    );
}

float Camera_worldToViewLength(const Camera* self, float length)
{
    const float scale = Camera_getWorldToViewScale(self);
    return length * self->scale.x;
}

void Camera_worldToView(const Camera* self, Vec2 position, float* x, float* y)
{
    assert(self && "self must not be NULL");
    assert(x && y);
    (*x) = Camera_worldToViewX(self, position.x);
    (*y) = Camera_worldToViewY(self, position.y);
}

void Camera_viewToWorld(const Camera* self, float x, float y, Vec2* position)
{
    assert(self && "self must not be NULL");
    assert(self->viewport && "viewport must not be NULL");

    SDL_FRect viewportRect = Viewport_getRect(self->viewport);
    const float ratioX = (x - viewportRect.x) / viewportRect.w;
    const float ratioY = (y - viewportRect.y) / viewportRect.h;
    const float w = self->worldView.upper.x - self->worldView.lower.x;
    const float h = self->worldView.upper.y - self->worldView.lower.y;
    position->x = self->worldView.lower.x + ratioX * w;
    position->y = self->worldView.lower.y + (1.f - ratioY) * h;
}

void Camera_setWorldView(Camera* self, AABB worldView)
{
    assert(self && "The Camera must be created");
    self->worldView = worldView;
}

void Camera_translateWorldView(Camera* self, Vec2 displacement)
{
    assert(self && "self must not be NULL");
    AABB_translate(&self->worldView, displacement);
}

float Camera_getWorldToViewScale(const Camera* self)
{
    assert(self && "self must not be NULL");
    return self->scale.x;
}
