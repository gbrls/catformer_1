#include <stdio.h>
#include <SDL2/SDL.h>
#include "types.h"

SDL_Window* window = NULL;
SDL_Surface* screen_surf = NULL;
SDL_Surface* sprite_surf = NULL;

typedef enum {
    WALING = 0,
    IDLE = 4,
    FALL = 6,
    JUMPING = 8,
    CLIMBING = 10,

}PlayerState;

u32 anim_duration(PlayerState state) {
    switch (state) {
    case WALING: return 4;
    case IDLE: return 4;
    case FALL: return 2;
    case JUMPING: return 2;
    case CLIMBING: return 2;
    default: break;
    }
}

typedef struct Player {
    float anim_frame;
    i64 xpos;
    i64 ypos;

    PlayerState state;

}Player;

void update_player(Player* p) {
    //p->anim_frame = (float)((u32)p->anim_frame % 4);
    p->anim_frame += 0.01;

}

u32 anim_frame(Player p) {
   return ((u32)p.anim_frame % anim_duration(p.state)) + p.state;

}

int check(void* data, char* message) {
    if(data == NULL){
        printf("%s: ", message);
        printf("(%s)\n", SDL_GetError());

        return -1;
    }

    return 0;
}

void close() {
    SDL_DestroyWindow(window);
    window = NULL;

    //Thorws core dumped
    //SDL_Quit();
}

int init(char* title, int w, int h) {

    //if(SDL_Init(SDL_INIT_EVERYTHING & (~SDL_INIT_AUDIO))< 0) {
    if(SDL_Init(SDL_INIT_VIDEO)< 0) {
        printf("Error starting sdl\n");

        return -1;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if(check((void*)window, "Error creating window") < 0) {
        return -1;
    }

    screen_surf = SDL_GetWindowSurface(window);
    if(check((void*)screen_surf, "Error creating screen surface") < 0) {
        return -1;
    }

    return 0;
}

int load_sprite(char* path) {

    sprite_surf = SDL_LoadBMP(path);
    if(check(sprite_surf, "Failed to load spritesheet")) {
        -1;
    }

    return 0;
}

int main(int argc, char** argv) {

    if(init("CatFormer", 800, 600) < 0) {
        printf("Coudn't init\n");
        return 0;
    }

    load_sprite("/home/gabriel/Programming/Games/Assets/0x72_8x8TilesetF24.v1.bmp");
    int quit = 0;
    SDL_Event e;

    Player player = {.anim_frame = 0, .xpos = 0, .ypos = 0, .state = IDLE};


    while(!quit) {

        SDL_FillRect(screen_surf, NULL, SDL_MapRGB(screen_surf->format, 0, 10, 10));

        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                quit = 1;
            }

            if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {

                case SDLK_ESCAPE:
                    quit = 1; break;
                default:
                    break;

                }
            }
        }

        update_player(&player);

        u32 frame = anim_frame(player);


        SDL_Rect src = {.x = (frame) * 8, .y = 0, .w = 8, .h = 8};
        SDL_Rect dst = {.x = 50, .y = 100, .w = 80, .h = 80};

        SDL_BlitScaled(sprite_surf, &src, screen_surf, &dst);
        SDL_UpdateWindowSurface(window);

    }


    close();

    return 0;
}
