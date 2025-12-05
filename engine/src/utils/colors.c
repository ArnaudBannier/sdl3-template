/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "utils/colors.h"

OpenColors g_colors = { 0 };


SDL_Color Color_lerp(SDL_Color a, SDL_Color b, float t)
{
    SDL_Color result = { 0 };
    result.r = (Uint8)(SDL_roundf(a.r + (b.r - a.r) * t));
    result.g = (Uint8)(SDL_roundf(a.g + (b.g - a.g) * t));
    result.b = (Uint8)(SDL_roundf(a.b + (b.b - a.b) * t));
    result.a = (Uint8)(SDL_roundf(a.a + (b.a - a.a) * t));
    return result;
}

void OpenColors_init(OpenColors* colors)
{
    colors->white = (SDL_Color){ 255, 255, 255, 255 };
    colors->black = (SDL_Color){ 0, 0, 0, 255 };
    colors->transparent = (SDL_Color){ 0, 0, 0, 0 };

    // Gray
    colors->gray0 = (SDL_Color){ 248, 249, 250, 255 };
    colors->gray1 = (SDL_Color){ 241, 243, 245, 255 };
    colors->gray2 = (SDL_Color){ 233, 236, 239, 255 };
    colors->gray3 = (SDL_Color){ 222, 226, 230, 255 };
    colors->gray4 = (SDL_Color){ 206, 212, 218, 255 };
    colors->gray5 = (SDL_Color){ 173, 181, 189, 255 };
    colors->gray6 = (SDL_Color){ 134, 142, 150, 255 };
    colors->gray7 = (SDL_Color){ 73, 80, 87, 255 };
    colors->gray8 = (SDL_Color){ 52, 58, 64, 255 };
    colors->gray9 = (SDL_Color){ 33, 37, 41, 255 };

    //  Red
    colors->red0 = (SDL_Color){ 255, 245, 245, 255 };
    colors->red1 = (SDL_Color){ 255, 227, 227, 255 };
    colors->red2 = (SDL_Color){ 255, 201, 201, 255 };
    colors->red3 = (SDL_Color){ 255, 168, 168, 255 };
    colors->red4 = (SDL_Color){ 255, 135, 135, 255 };
    colors->red5 = (SDL_Color){ 255, 107, 107, 255 };
    colors->red6 = (SDL_Color){ 250, 82, 82, 255 };
    colors->red7 = (SDL_Color){ 240, 62, 62, 255 };
    colors->red8 = (SDL_Color){ 224, 49, 49, 255 };
    colors->red9 = (SDL_Color){ 201, 42, 42, 255 };


    // Pink
    colors->pink0 = (SDL_Color){ 255, 240, 246, 255 };
    colors->pink1 = (SDL_Color){ 255, 222, 235, 255 };
    colors->pink2 = (SDL_Color){ 252, 194, 215, 255 };
    colors->pink3 = (SDL_Color){ 250, 162, 193, 255 };
    colors->pink4 = (SDL_Color){ 247, 131, 172, 255 };
    colors->pink5 = (SDL_Color){ 240, 101, 149, 255 };
    colors->pink6 = (SDL_Color){ 230, 73, 128, 255 };
    colors->pink7 = (SDL_Color){ 214, 51, 108, 255 };
    colors->pink8 = (SDL_Color){ 194, 37, 92, 255 };
    colors->pink9 = (SDL_Color){ 166, 30, 77, 255 };

    // Grape
    colors->grape0 = (SDL_Color){ 248, 240, 252, 255 };
    colors->grape1 = (SDL_Color){ 243, 217, 250, 255 };
    colors->grape2 = (SDL_Color){ 238, 190, 250, 255 };
    colors->grape3 = (SDL_Color){ 229, 153, 247, 255 };
    colors->grape4 = (SDL_Color){ 218, 119, 242, 255 };
    colors->grape5 = (SDL_Color){ 204, 93, 232, 255 };
    colors->grape6 = (SDL_Color){ 190, 75, 219, 255 };
    colors->grape7 = (SDL_Color){ 174, 62, 201, 255 };
    colors->grape8 = (SDL_Color){ 156, 54, 181, 255 };
    colors->grape9 = (SDL_Color){ 134, 46, 156, 255 };

    // Violet
    colors->violet0 = (SDL_Color){ 243, 240, 255, 255 };
    colors->violet1 = (SDL_Color){ 229, 219, 255, 255 };
    colors->violet2 = (SDL_Color){ 208, 191, 255, 255 };
    colors->violet3 = (SDL_Color){ 177, 151, 252, 255 };
    colors->violet4 = (SDL_Color){ 151, 117, 250, 255 };
    colors->violet5 = (SDL_Color){ 132, 94, 247, 255 };
    colors->violet6 = (SDL_Color){ 121, 80, 242, 255 };
    colors->violet7 = (SDL_Color){ 112, 72, 232, 255 };
    colors->violet8 = (SDL_Color){ 103, 65, 217, 255 };
    colors->violet9 = (SDL_Color){ 95, 61, 196, 255 };

    // Indigo
    colors->indigo0 = (SDL_Color){ 237, 242, 255, 255 };
    colors->indigo1 = (SDL_Color){ 219, 228, 255, 255 };
    colors->indigo2 = (SDL_Color){ 186, 200, 255, 255 };
    colors->indigo3 = (SDL_Color){ 145, 167, 255, 255 };
    colors->indigo4 = (SDL_Color){ 116, 143, 252, 255 };
    colors->indigo5 = (SDL_Color){ 92, 124, 250, 255 };
    colors->indigo6 = (SDL_Color){ 76, 110, 245, 255 };
    colors->indigo7 = (SDL_Color){ 66, 99, 235, 255 };
    colors->indigo8 = (SDL_Color){ 59, 91, 219, 255 };
    colors->indigo9 = (SDL_Color){ 54, 79, 199, 255 };

    // Blue
    colors->blue0 = (SDL_Color){ 231, 245, 255, 255 };
    colors->blue1 = (SDL_Color){ 208, 235, 255, 255 };
    colors->blue2 = (SDL_Color){ 165, 216, 255, 255 };
    colors->blue3 = (SDL_Color){ 116, 192, 252, 255 };
    colors->blue4 = (SDL_Color){ 77, 171, 247, 255 };
    colors->blue5 = (SDL_Color){ 51, 154, 240, 255 };
    colors->blue6 = (SDL_Color){ 34, 139, 230, 255 };
    colors->blue7 = (SDL_Color){ 28, 126, 214, 255 };
    colors->blue8 = (SDL_Color){ 25, 113, 194, 255 };
    colors->blue9 = (SDL_Color){ 24, 100, 171, 255 };

    // Cyan
    colors->cyan0 = (SDL_Color){ 227, 250, 252, 255 };
    colors->cyan1 = (SDL_Color){ 197, 246, 250, 255 };
    colors->cyan2 = (SDL_Color){ 153, 233, 242, 255 };
    colors->cyan3 = (SDL_Color){ 102, 217, 232, 255 };
    colors->cyan4 = (SDL_Color){ 59, 201, 219, 255 };
    colors->cyan5 = (SDL_Color){ 34, 184, 207, 255 };
    colors->cyan6 = (SDL_Color){ 21, 170, 191, 255 };
    colors->cyan7 = (SDL_Color){ 16, 152, 173, 255 };
    colors->cyan8 = (SDL_Color){ 12, 133, 153, 255 };
    colors->cyan9 = (SDL_Color){ 11, 114, 133, 255 };

    // Teal
    colors->teal0 = (SDL_Color){ 230, 252, 245, 255 };
    colors->teal1 = (SDL_Color){ 195, 250, 232, 255 };
    colors->teal2 = (SDL_Color){ 150, 242, 215, 255 };
    colors->teal3 = (SDL_Color){ 99, 230, 190, 255 };
    colors->teal4 = (SDL_Color){ 56, 217, 169, 255 };
    colors->teal5 = (SDL_Color){ 32, 201, 151, 255 };
    colors->teal6 = (SDL_Color){ 18, 184, 134, 255 };
    colors->teal7 = (SDL_Color){ 12, 166, 120, 255 };
    colors->teal8 = (SDL_Color){ 9, 146, 104, 255 };
    colors->teal9 = (SDL_Color){ 8, 127, 91, 255 };


    // Green
    colors->green0 = (SDL_Color){ 235, 251, 238, 255 };
    colors->green1 = (SDL_Color){ 211, 249, 216, 255 };
    colors->green2 = (SDL_Color){ 178, 242, 187, 255 };
    colors->green3 = (SDL_Color){ 140, 233, 154, 255 };
    colors->green4 = (SDL_Color){ 105, 219, 124, 255 };
    colors->green5 = (SDL_Color){ 81, 207, 102, 255 };
    colors->green6 = (SDL_Color){ 64, 192, 87, 255 };
    colors->green7 = (SDL_Color){ 55, 178, 77, 255 };
    colors->green8 = (SDL_Color){ 47, 158, 68, 255 };
    colors->green9 = (SDL_Color){ 43, 138, 62, 255 };


    // Lime
    colors->lime0 = (SDL_Color){ 244, 252, 227, 255 };
    colors->lime1 = (SDL_Color){ 233, 250, 200, 255 };
    colors->lime2 = (SDL_Color){ 216, 245, 162, 255 };
    colors->lime3 = (SDL_Color){ 192, 235, 117, 255 };
    colors->lime4 = (SDL_Color){ 169, 227, 75, 255 };
    colors->lime5 = (SDL_Color){ 148, 216, 45, 255 };
    colors->lime6 = (SDL_Color){ 130, 201, 30, 255 };
    colors->lime7 = (SDL_Color){ 116, 184, 22, 255 };
    colors->lime8 = (SDL_Color){ 102, 168, 15, 255 };
    colors->lime9 = (SDL_Color){ 92, 148, 13, 255 };

    // Yellow
    colors->yellow0 = (SDL_Color){ 255, 249, 219, 255 };
    colors->yellow1 = (SDL_Color){ 255, 243, 191, 255 };
    colors->yellow2 = (SDL_Color){ 255, 236, 153, 255 };
    colors->yellow3 = (SDL_Color){ 255, 224, 102, 255 };
    colors->yellow4 = (SDL_Color){ 255, 212, 59, 255 };
    colors->yellow5 = (SDL_Color){ 252, 196, 25, 255 };
    colors->yellow6 = (SDL_Color){ 250, 176, 5, 255 };
    colors->yellow7 = (SDL_Color){ 245, 159, 0, 255 };
    colors->yellow8 = (SDL_Color){ 240, 140, 0, 255 };
    colors->yellow9 = (SDL_Color){ 230, 119, 0, 255 };

    // Orange
    colors->orange0 = (SDL_Color){ 255, 244, 230, 255 };
    colors->orange1 = (SDL_Color){ 255, 232, 204, 255 };
    colors->orange2 = (SDL_Color){ 255, 216, 168, 255 };
    colors->orange3 = (SDL_Color){ 255, 192, 120, 255 };
    colors->orange4 = (SDL_Color){ 255, 169, 77, 255 };
    colors->orange5 = (SDL_Color){ 255, 146, 43, 255 };
    colors->orange6 = (SDL_Color){ 253, 126, 20, 255 };
    colors->orange7 = (SDL_Color){ 247, 103, 7, 255 };
    colors->orange8 = (SDL_Color){ 232, 89, 12, 255 };
    colors->orange9 = (SDL_Color){ 217, 72, 15, 255 };
}
