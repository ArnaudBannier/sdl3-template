/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288f
#endif
#define RAD_TO_DEG 57.295779513082320876798154814105f
#define DEG_TO_RAD 0.01745329251994329576923690768489f

/// @brief Structure représentant un vecteur de R^2.
typedef struct Vec2
{
    float x;
    float y;
} Vec2;

extern const Vec2 Vec2_up;
extern const Vec2 Vec2_down;
extern const Vec2 Vec2_left;
extern const Vec2 Vec2_right;
extern const Vec2 Vec2_zero;
extern const Vec2 Vec2_one;

extern const Vec2 Vec2_anchor_north_west;
extern const Vec2 Vec2_anchor_north;
extern const Vec2 Vec2_anchor_north_east;
extern const Vec2 Vec2_anchor_west;
extern const Vec2 Vec2_anchor_center;
extern const Vec2 Vec2_anchor_east;
extern const Vec2 Vec2_anchor_south_west;
extern const Vec2 Vec2_anchor_south;
extern const Vec2 Vec2_anchor_south_east;

/// @brief Construit un vecteur à partir de ses composantes.
/// @param x la composante x.
/// @param y la composante y.
/// @return Le vecteur ayant les composantes données.
Vec2 Vec2_set(float x, float y);

/// @brief Additionne deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La somme de v1 et v2.
Vec2 Vec2_add(Vec2 v1, Vec2 v2);

/// @brief Soustrait deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La différence de v1 par v2.
Vec2 Vec2_sub(Vec2 v1, Vec2 v2);

Vec2 Vec2_mul(Vec2 v1, Vec2 v2);
Vec2 Vec2_div(Vec2 v1, Vec2 v2);

/// @brief Multiplie un vecteur par un scalaire.
/// @param v le vecteur.
/// @param s le scalaire.
/// @return Le produit de s et v.
Vec2 Vec2_scale(Vec2 v, float s);

/// @brief Renvoie la longueur au carré d'un vecteur.
/// @param v le vecteur.
/// @return La longeur au carré du vecteur.
float Vec2_lengthSquared(Vec2 v);

/// @brief Renvoie la longueur (norme euclidienne) d'un vecteur.
/// @param v le vecteur.
/// @return La norme euclidienne de v.
float Vec2_length(Vec2 v);

/// @brief Normalise un vecteur.
/// @param v le vecteur.
/// @return Le vecteur unitaire de même direction.
Vec2 Vec2_normalize(Vec2 v);

/// @brief Renvoie la distance au carré entre deux points.
/// @param v1 les coordonnées du premier point.
/// @param v2 les coordonnées du second point.
/// @return La distance au carré séparant les deux points.
float Vec2_distanceSquared(Vec2 v1, Vec2 v2);

/// @brief Renvoie la distance entre deux points.
/// @param v1 les coordonnées du premier point.
/// @param v2 les coordonnées du second point.
/// @return La distance séparant les deux points.
float Vec2_distance(Vec2 v1, Vec2 v2);

/// @brief Renvoie le produit scalaire entre deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le produit scalaire entre v1 et v2.
float Vec2_dot(Vec2 v1, Vec2 v2);

/// @brief Renvoie le déterminant d'une matrice 2x2.
/// @param v1 la première colonne de la matrice.
/// @param v2 la seconde colonne de la matrice.
/// @return Le déterminant de la matrice.
float Vec2_det(Vec2 v1, Vec2 v2);

/// @brief Renvoie le vecteur orthogonal à un vecteur obtenu
/// par rotation de 90 degrés dans le sens trigonométrique.
/// @param v le vecteur.
/// @return Le vecteur orthogonal à v dans le sens direct.
Vec2 Vec2_perp(Vec2 v);

/// @brief Renvoie l'angle (signé, orienté dans le sens trigonométrique) formé entre deux vecteurs.
/// L'angle est exprimé en degrés (de -180 à 180).
/// @param from le vecteur de départ.
/// @param to le vecteur d'arrivée.
/// @return L'angle orienté du vecteur from vers le vecteur to.
float Vec2_signedAngleDeg(Vec2 from, Vec2 to);

/// @brief Renvoie l'angle (signé, orienté dans le sens trigonométrique) formé entre deux vecteurs.
/// L'angle est exprimé en radians (de -pi à pi).
/// @param from le vecteur de départ.
/// @param to le vecteur d'arrivée.
/// @return L'angle orienté du vecteur from vers le vecteur to.
float Vec2_signedAngleRad(Vec2 from, Vec2 to);

/// @brief Modifie progressivement un vecteur vers un objectif souhaité au fil du temps.
/// @param current la position courante.
/// @param target la position souahitée.
/// @param currentVelocity vitesse courante.
///     Cette valeur est modifiée à chaque appel de la fonction.
/// @param smoothTime temps approximatif nécessaire pour atteindre la cible. 
///     Une valeur plus petite permettra d'atteindre la cible plus rapidement.
/// @param maxSpeed permet de limiter la vitesse maximale.
/// @param deltaTime temps écoulé depuis le dernier appel à cette fonction.
/// @return La nouvelle valeur du vecteur.
Vec2 Vec2_smoothDamp(
    Vec2 current, Vec2 target, Vec2* currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime
);

/// @brief Structure représentant une boîte en deux dimensions alignées sur les axes x et y.
/// AABB signifie "Axis-Aligned Bounding Box".
typedef struct AABB
{
    /// @brief Coordonnées du coin inférieur gauche de la boîte.
    Vec2 lower;

    /// @brief Coordonnées du coin supérieur droit de la boîte.
    Vec2 upper;
} AABB;

/// @brief Définit une boîte.
/// @param xLower composante x minimale de la boîte.
/// @param yLower composante y minimale de la boîte.
/// @param xUpper composante x maximale de la boîte.
/// @param yUpper composante y maximale de la boîte.
/// @return La boîte créée.
INLINE AABB AABB_set(float xLower, float yLower, float xUpper, float yUpper)
{
    AABB aabb = { 0 };
    aabb.lower = Vec2_set(xLower, yLower);
    aabb.upper = Vec2_set(xUpper, yUpper);
    return aabb;
}

/// @brief Applique une translation à une boîte.
/// @param aabb pointeur vers la boîte à transformer.
/// @param transform vecteur de translation.
INLINE void AABB_translate(AABB* aabb, Vec2 transform)
{
    aabb->lower = Vec2_add(aabb->lower, transform);
    aabb->upper = Vec2_add(aabb->upper, transform);
}

INLINE Vec2 AABB_getCenter(const AABB* aabb)
{
    Vec2 center = {
        0.5f * (aabb->lower.x + aabb->upper.x),
        0.5f * (aabb->lower.y + aabb->upper.y)
    };
    return center;
}

INLINE Vec2 AABB_getExtents(const AABB* aabb)
{
    Vec2 extents = {
        0.5f * (aabb->upper.x - aabb->lower.x),
        0.5f * (aabb->upper.y - aabb->lower.y)
    };
    return extents;
}

INLINE Vec2 AABB_getSize(const AABB* aabb)
{
    Vec2 size = {
        aabb->upper.x - aabb->lower.x,
        aabb->upper.y - aabb->lower.y
    };
    return size;
}

INLINE bool AABB_containsPoint(const AABB* aabb, Vec2 point)
{
    return
        (aabb->lower.x <= point.x) && (point.x <= aabb->upper.x) &&
        (aabb->lower.y <= point.y) && (point.y <= aabb->upper.y);
}

Vec2 AABB_shortestVector(const AABB* a, const AABB* b);

/// @brief Modifie progressivement un réel vers un objectif souhaité au fil du temps.
/// @param current la valeur courante.
/// @param target la valeur souahitée.
/// @param currentVelocity vitesse courante.
///     Cette valeur est modifiée à chaque appel de la fonction.
/// @param smoothTime temps approximatif nécessaire pour atteindre la cible. 
///     Une valeur plus petite permettra d'atteindre la cible plus rapidement.
/// @param maxSpeed permet de limiter la vitesse maximale.
/// @param deltaTime temps écoulé depuis le dernier appel à cette fonction.
/// @return La nouvelle valeur.
float Float_smoothDamp(
    float current, float target, float* currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime);

/// @brief Calcule une approximation linéaire entre deux valeurs.
/// Le paramètre d'interpolation est automatiquement borné entre 0 et 1.
/// @param a la première valeur (correspondant à t = 0).
/// @param b la seconde valeur (correspondant à t = 1).
/// @param t le paramètre d'interpolation.
/// @return L'interpolation linéaire entre a et b.
INLINE float Float_lerp(float a, float b, float t)
{
    t = fmaxf(0.f, fminf(1.f, t));
    return (1.0f - t) * a + t * b;
}

/// @brief Calcule une approximation linéaire entre deux valeurs.
/// @param a la première valeur (correspondant à t = 0).
/// @param b la seconde valeur (correspondant à t = 1).
/// @param t le paramètre d'interpolation.
/// @return L'interpolation linéaire entre a et b.
INLINE float Float_lerpUnclamped(float a, float b, float t)
{
    return (1.0f - t) * a + t * b;
}

INLINE bool FRect_containsPoint(const SDL_FRect* rect, Vec2 p)
{
    return
        (rect->x <= p.x) && (p.x <= rect->x + rect->w) &&
        (rect->y <= p.y) && (p.y <= rect->y + rect->h);
}

void FRect_projectOntoAxis(const SDL_FRect* rect, Vec2 axis, float* outMin, float* outMax);
INLINE Vec2 FRect_getCenter(const SDL_FRect* rect)
{
    Vec2 center = {
        rect->x + rect->w * 0.5f,
        rect->y + rect->h * 0.5f
    };
    return center;
}

/// @brief Borne un entier entre une valeur minimale et une valeur maximale.
/// @param value Valeur à borner.
/// @param a Valeur minimale.
/// @param b Valeur maximale.
/// @return La valeur a si (value < a), b si (value > b) et value sinon.
INLINE int Int_clamp(int value, int a, int b)
{
    value = (value > a) ? value : a;
    value = (value < b) ? value : b;
    return value;
}

/// @brief Retourne le minimum de deux entiers.
/// @param a Premier entier.
/// @param b Second entier.
/// @return Le plus petit des deux entiers.
INLINE int Int_min(int a, int b)
{
    return (a < b) ? a : b;
}

/// @brief Retourne le maximum de deux entiers.
/// @param a Premier entier.
/// @param b Second entier.
/// @return Le plus grand des deux entiers.
INLINE int Int_max(int a, int b)
{
    return (a > b) ? a : b;
}

/// @brief Génère un entier aléatoire.
/// @param a Valeur minimale (incluse).
/// @param b Valeur maximale (exclue).
/// @return Un entier aléatoire compris entre a (inclus) et b (exclus).
INLINE int Int_randAB(int a, int b)
{
    return a + (rand() % (b - a));
}

/// @brief Génère un flottant aléatoire entre 0.f et 1.f.
/// @return Un flottant aléatoire compris entre 0.f (inclus) et 1.f (inclus).
INLINE float Float_rand01()
{
    return (float)rand() / (float)(RAND_MAX);
}

/// @brief Génère un flottant aléatoire.
/// @param a Valeur minimale.
/// @param b Valeur maximale.
/// @return Un flottant aléatoire compris entre a et b.
INLINE float Float_randAB(float a, float b)
{
    return a + (b - a) * Float_rand01();
}

/// @brief Borne un flottant entre une valeur minimale et une valeur maximale.
/// @param value Valeur à borner.
/// @param a Valeur minimale.
/// @param b Valeur maximale.
/// @return Retourne a si value < a, b si value > b, et value sinon.
INLINE float Float_clamp(float value, float a, float b)
{
    return fmaxf(a, fminf(value, b));
}

/// @brief Borne un flottant entre 0.f et 1.f.
/// @param value Valeur à borner.
/// @return Retourne 0.f si value < 0.f, 1.f si value > 1.f, et value sinon.
INLINE float Float_clamp01(float value)
{
    return Float_clamp(value, 0.0f, 1.0f);
}
