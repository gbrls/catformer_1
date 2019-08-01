#include <SDL2/SDL.h>
#include "../include/pixel.h"
#include "../include/types.h"

u32 get_pixel(SDL_Surface* surface, u32 x, u32 y) {
    u32* pixels = (u32*)surface->pixels;

    if(x >= surface->w || y >= surface->h) {
        return -1;
    }

    return pixels[x + (y * surface->w)];
}



void set_pixel(SDL_Surface* surface, u32 x, u32 y, u32 pixel) {
    u32* pixels = (u32*)surface->pixels;

    if(x >= surface->w || y >= surface->h) {
        return;
    }

    pixels[x + (y * surface->w)] = pixel;
}
