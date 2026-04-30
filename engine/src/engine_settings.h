/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#define MAGIC_NUMBER_UI_OBJECT (uint64_t)0xAABBCCDDEEFF0000

#ifdef _MSC_VER
#  define INLINE inline
#else
#  define INLINE static inline
#endif

#define ASSERT_NEW(ptr) { if (ptr == NULL) { assert(false); abort(); } }

#define CHECK_SDL_SUCCESS(expr, category) \
    if (!(expr)) {                        \
        SDL_LogError(                     \
            category,                     \
            "%s:%d (%s)",                 \
            __FILE__,                     \
            __LINE__,                     \
            __func__                      \
        );                                \
        SDL_LogError(                     \
            category,                     \
            "%s",                         \
            SDL_GetError()                \
        );                                \
        assert(false);                    \
    }
