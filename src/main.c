#include <stdio.h>
#include <SDL2/SDL.h>
#include "types.h"

SDL_Window* window = NULL;
SDL_Surface* screen_surf = NULL;
SDL_Surface* sprite_surf = NULL;

float PLAYER_SPEED = 0.1;
float JUMP_SPEED = -1;
float JUMP_DES = 0.005;

float FLOOR = 120;

typedef enum {
    WALING = 0,
    IDLE = 4,
    FALL = 8,
    JUMPING = 10,
    CLIMBING = 12,

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
    float xpos;
    float ypos;

    float xspeed;
    float yspeed;

    PlayerState state;

}Player;

u32 anim_frame(Player p) {
    return ((u32)p.anim_frame % anim_duration(p.state)) + p.state;
}

void player_fall(Player* p){

    //printf("%0.2f %0.2f", p->ypos, p->yspeed);
    if(p->yspeed > 0) {
        p->state = FALL;
    }

    if((p->ypos + p->yspeed) > FLOOR){
        p->yspeed = 0;
        p->ypos = FLOOR;

        p->state = IDLE;

        return;
    }

    p->ypos += p->yspeed;

    p->yspeed += JUMP_DES;
}

void update_player(Player* p) {
    //p->anim_frame = (float)((u32)p->anim_frame % 4);
    p->anim_frame += 0.01;

    p->xpos += p->xspeed;

    switch(p->state){
    case FALL: player_fall(p); break;
    case JUMPING: player_fall(p); break;
    default: break;
    }

}

void player_keydown(Player *p, SDL_Keycode key) {
    switch (p->state){

    case IDLE: switch(key){
        case SDLK_d: p->xspeed = PLAYER_SPEED;p->state = WALING; break;
        case SDLK_w: p->yspeed = JUMP_SPEED;p->state = JUMPING; break;

        default: printf("code: %d\n", key); break;
        }; break;

    default: break;
    }
}

void player_keyup(Player *p, SDL_Keycode key) {
    switch (p->state){

    case WALING: switch(key){
        case SDLK_d: p->xspeed = 0; p->state = IDLE; break;

        default: printf("code: %d\n", key); break;
        }; break;

    default: break;
    }
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

    Player player = {.anim_frame = 0, .xpos = 0, .ypos = FLOOR, .state = IDLE};


    while(!quit) {

        SDL_FillRect(screen_surf, NULL, SDL_MapRGB(screen_surf->format, 0, 10, 10));

        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                quit = 1;
            }

            //*TODO create a key buffer, to use multiple keys
            if(e.type == SDL_KEYDOWN) {

                player_keydown(&player, e.key.keysym.sym);

                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = 1; break;
                default: break;
                }
            }

            if(e.type == SDL_KEYUP) {
                player_keyup(&player, e.key.keysym.sym);
            }
        }

        update_player(&player);

        u32 frame = anim_frame(player);


        SDL_Rect src = {.x = (frame) * 8, .y = 0, .w = 8, .h = 8};
        SDL_Rect dst = {.x = (int)player.xpos, .y = (int)player.ypos, .w = 80, .h = 80};

        SDL_BlitScaled(sprite_surf, &src, screen_surf, &dst);
        SDL_UpdateWindowSurface(window);

    }


    close();

    return 0;
}
