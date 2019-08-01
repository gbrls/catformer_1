#pragma once

#include <SDL2/SDL.h>
#include "types.h"

u32 get_pixel(SDL_Surface* surface, u32 x, u32 y);

void set_pixel(SDL_Surface* surface, u32 x, u32 y, u32 pixel);
