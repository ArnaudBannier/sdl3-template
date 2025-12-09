/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "game_engine_settings.h"

// https://github.com/yeun/open-color
typedef struct OpenColors OpenColors;

extern OpenColors g_colors;

void OpenColors_init(OpenColors* colors);

SDL_Color Color_lerp(SDL_Color a, SDL_Color b, float t);

struct OpenColors
{
    SDL_Color black;
    SDL_Color white;
    SDL_Color transparent;

    SDL_Color gray0;
    SDL_Color gray1;
    SDL_Color gray2;
    SDL_Color gray3;
    SDL_Color gray4;
    SDL_Color gray5;
    SDL_Color gray6;
    SDL_Color gray7;
    SDL_Color gray8;
    SDL_Color gray9;

    SDL_Color red0;
    SDL_Color red1;
    SDL_Color red2;
    SDL_Color red3;
    SDL_Color red4;
    SDL_Color red5;
    SDL_Color red6;
    SDL_Color red7;
    SDL_Color red8;
    SDL_Color red9;

    SDL_Color pink0;
    SDL_Color pink1;
    SDL_Color pink2;
    SDL_Color pink3;
    SDL_Color pink4;
    SDL_Color pink5;
    SDL_Color pink6;
    SDL_Color pink7;
    SDL_Color pink8;
    SDL_Color pink9;

    SDL_Color grape0;
    SDL_Color grape1;
    SDL_Color grape2;
    SDL_Color grape3;
    SDL_Color grape4;
    SDL_Color grape5;
    SDL_Color grape6;
    SDL_Color grape7;
    SDL_Color grape8;
    SDL_Color grape9;

    SDL_Color violet0;
    SDL_Color violet1;
    SDL_Color violet2;
    SDL_Color violet3;
    SDL_Color violet4;
    SDL_Color violet5;
    SDL_Color violet6;
    SDL_Color violet7;
    SDL_Color violet8;
    SDL_Color violet9;

    SDL_Color indigo0;
    SDL_Color indigo1;
    SDL_Color indigo2;
    SDL_Color indigo3;
    SDL_Color indigo4;
    SDL_Color indigo5;
    SDL_Color indigo6;
    SDL_Color indigo7;
    SDL_Color indigo8;
    SDL_Color indigo9;

    SDL_Color blue0;
    SDL_Color blue1;
    SDL_Color blue2;
    SDL_Color blue3;
    SDL_Color blue4;
    SDL_Color blue5;
    SDL_Color blue6;
    SDL_Color blue7;
    SDL_Color blue8;
    SDL_Color blue9;

    SDL_Color cyan0;
    SDL_Color cyan1;
    SDL_Color cyan2;
    SDL_Color cyan3;
    SDL_Color cyan4;
    SDL_Color cyan5;
    SDL_Color cyan6;
    SDL_Color cyan7;
    SDL_Color cyan8;
    SDL_Color cyan9;

    SDL_Color teal0;
    SDL_Color teal1;
    SDL_Color teal2;
    SDL_Color teal3;
    SDL_Color teal4;
    SDL_Color teal5;
    SDL_Color teal6;
    SDL_Color teal7;
    SDL_Color teal8;
    SDL_Color teal9;

    SDL_Color green0;
    SDL_Color green1;
    SDL_Color green2;
    SDL_Color green3;
    SDL_Color green4;
    SDL_Color green5;
    SDL_Color green6;
    SDL_Color green7;
    SDL_Color green8;
    SDL_Color green9;

    SDL_Color lime0;
    SDL_Color lime1;
    SDL_Color lime2;
    SDL_Color lime3;
    SDL_Color lime4;
    SDL_Color lime5;
    SDL_Color lime6;
    SDL_Color lime7;
    SDL_Color lime8;
    SDL_Color lime9;

    SDL_Color yellow0;
    SDL_Color yellow1;
    SDL_Color yellow2;
    SDL_Color yellow3;
    SDL_Color yellow4;
    SDL_Color yellow5;
    SDL_Color yellow6;
    SDL_Color yellow7;
    SDL_Color yellow8;
    SDL_Color yellow9;

    SDL_Color orange0;
    SDL_Color orange1;
    SDL_Color orange2;
    SDL_Color orange3;
    SDL_Color orange4;
    SDL_Color orange5;
    SDL_Color orange6;
    SDL_Color orange7;
    SDL_Color orange8;
    SDL_Color orange9;
};
