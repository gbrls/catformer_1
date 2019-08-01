#pragma once
#include<SDL2/SDL.h>
#include "./types.h"

static int exists(u32 number);
static void draw_number(u32 number, u32 offset, u32 color, SDL_Surface* surface);
void load_font(SDL_Surface* font_surface);
SDL_Surface* render_text(char* text, u32 color);
