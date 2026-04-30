/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "utils/utils.h"

const Vec2 Vec2_up = { 0.0f,  1.0f };
const Vec2 Vec2_down = { 0.0f, -1.0f };
const Vec2 Vec2_left = { -1.0f,  0.0f };
const Vec2 Vec2_right = { 1.0f,  0.0f };
const Vec2 Vec2_zero = { 0.0f,  0.0f };
const Vec2 Vec2_one = { 1.0f,  1.0f };

const Vec2 Vec2_anchor_north_west = { 0.0f, 1.0f };
const Vec2 Vec2_anchor_north = { 0.5f, 1.0f };
const Vec2 Vec2_anchor_north_east = { 1.0f, 1.0f };
const Vec2 Vec2_anchor_west = { 0.0f, 0.5f };
const Vec2 Vec2_anchor_center = { 0.5f, 0.5f };
const Vec2 Vec2_anchor_east = { 1.0f, 0.5f };
const Vec2 Vec2_anchor_south_west = { 0.0f, 0.0f };
const Vec2 Vec2_anchor_south = { 0.5f, 0.0f };
const Vec2 Vec2_anchor_south_east = { 1.0f, 0.0f };

Vec2 Vec2_set(float x, float y)
{
    Vec2 v = { .x = x, .y = y };
    return v;
}

Vec2 Vec2_add(Vec2 v1, Vec2 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

Vec2 Vec2_sub(Vec2 v1, Vec2 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}

Vec2 Vec2_mul(Vec2 v1, Vec2 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}

Vec2 Vec2_div(Vec2 v1, Vec2 v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}

Vec2 Vec2_scale(Vec2 v, float s)
{
    v.x *= s;
    v.y *= s;
    return v;
}

float Vec2_lengthSquared(Vec2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

float Vec2_length(Vec2 v)
{
    return sqrtf(Vec2_lengthSquared(v));
}

Vec2 Vec2_normalize(Vec2 v)
{
    float norm = Vec2_length(v);

    if (fabsf(norm) > 1e-5)
    {
        v.x /= norm;
        v.y /= norm;
    }
    else
    {
        v.x = 1.f;
        v.y = 0.f;
    }

    return v;
}

float Vec2_distanceSquared(Vec2 v1, Vec2 v2)
{
    return Vec2_lengthSquared(Vec2_sub(v1, v2));
}

float Vec2_distance(Vec2 v1, Vec2 v2)
{
    return Vec2_length(Vec2_sub(v1, v2));
}

Vec2 Vec2_perp(Vec2 v)
{
    return Vec2_set(v.y, -v.x);
}

static float Vec2_signedAngleAtan2(Vec2 from, Vec2 to)
{
    // Le calcule de l'angle est plus précis avec atan2 plutôt qu'avec acos
    // Il n'y a pas non plus besoin de normaliser les vecteurs
    float s = Vec2_det(from, to); // |from|.|to|.sin(angle)
    float c = Vec2_dot(from, to); // |from|.|to|.cos(angle)
    return atan2f(s, c);
}

float Vec2_signedAngleDeg(Vec2 from, Vec2 to)
{
    return Vec2_signedAngleAtan2(from, to) * RAD_TO_DEG;
}

float Vec2_signedAngleRad(Vec2 from, Vec2 to)
{
    return Vec2_signedAngleAtan2(from, to);
}

Vec2 Vec2_rotateDeg(Vec2 v, float angleDeg)
{
    float angleRad = angleDeg * DEG_TO_RAD;
    float cosA = cosf(angleRad);
    float sinA = sinf(angleRad);
    Vec2 res = {
        .x = v.x * cosA - v.y * sinA,
        .y = v.x * sinA + v.y * cosA
    };
    return res;
}

Vec2 Vec2_rotateRad(Vec2 v, float angleRad)
{
    float cosA = cosf(angleRad);
    float sinA = sinf(angleRad);
    Vec2 res = {
        .x = v.x * cosA - v.y * sinA,
        .y = v.x * sinA + v.y * cosA
    };
    return res;
}

float Vec2_det(Vec2 v1, Vec2 v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

float Vec2_dot(Vec2 v1, Vec2 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

Vec2 Vec2_smoothDamp(
    Vec2 current, Vec2 target, Vec2* currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime)
{
    Vec2 res = { 0 };

    res.x = Float_smoothDamp(
        current.x, target.x, &(currentVelocity->x),
        smoothTime, maxSpeed, deltaTime);

    res.y = Float_smoothDamp(
        current.y, target.y, &(currentVelocity->y),
        smoothTime, maxSpeed, deltaTime);

    return res;
}

Vec2 Vec2_lerp(Vec2 a, Vec2 b, float t)
{
    const float u = 1.0f - t;
    a.x = u * a.x + t * b.x;
    a.y = u * a.y + t * b.y;
    return a;
}

Vec2 AABB_shortestVector(const AABB* a, const AABB* b)
{
    Vec2 shortest = Vec2_zero;

    if (a->upper.x < b->lower.x)
    {
        shortest.x = b->lower.x - a->upper.x;
    }
    else if (b->upper.x < a->lower.x)
    {
        shortest.x = b->upper.x - a->lower.x;
    }

    if (a->upper.y < b->lower.y)
    {
        shortest.y = b->lower.y - a->upper.y;
    }
    else if (b->upper.y < a->lower.y)
    {
        shortest.y = b->upper.y - a->lower.y;
    }

    return shortest;
}

float Float_smoothDamp(
    float current, float target, float* currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime)
{
    // Implémentation de Unity
    // Voir Game Programming Gems 4 chapitre 1.10 page 99
    smoothTime = fmaxf(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float targetCopy = target;
    float maxChange = maxSpeed * smoothTime;
    change = fmaxf(-maxChange, fminf(+maxChange, change));
    target = current - change;
    float temp = (*currentVelocity + omega * change) * deltaTime;
    *currentVelocity = (*currentVelocity - omega * temp) * exp;
    float res = target + (change + temp) * exp;
    if ((targetCopy - current > 0.0f) == (res > targetCopy))
    {
        res = targetCopy;
        *currentVelocity = (res - targetCopy) / deltaTime;
    }
    return res;
}

void FRect_projectOntoAxis(
    const SDL_FRect* rect, Vec2 axis,
    float* outMin, float* outMax)
{
    Vec2 corners[4] = {
        Vec2_set(rect->x, rect->y),
        Vec2_set(rect->x + rect->w, rect->y),
        Vec2_set(rect->x, rect->y + rect->h),
        Vec2_set(rect->x + rect->w, rect->y + rect->h)
    };

    *outMin = *outMax = Vec2_dot(corners[0], axis);
    for (int i = 1; i < 4; i++)
    {
        float projection = Vec2_dot(corners[i], axis);
        *outMin = fminf(*outMin, projection);
        *outMax = fmaxf(*outMax, projection);
    }
}

uint64_t Uint64_hash(uint64_t value)
{
    uint64_t z = value + 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return z;
}
