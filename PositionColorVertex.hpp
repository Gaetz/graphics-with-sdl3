//
// Created by Gaëtan Blaise-Cazalet on 20/11/2024.
//

#ifndef GRAPHICS_SDL3_POSITIONCOLORVERTEX_HPP
#define GRAPHICS_SDL3_POSITIONCOLORVERTEX_HPP

#include <SDL3/SDL_stdinc.h>

typedef struct PositionColorVertex
{
    float x, y, z;
    Uint8 r, g, b, a;
} PositionColorVertex;

#endif //GRAPHICS_SDL3_POSITIONCOLORVERTEX_HPP
