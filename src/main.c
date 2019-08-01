#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../include/types.h"
#include "../include/pixel.h"
#include "../include/font.h"

SDL_Window* window = NULL;
SDL_Surface* screen_surf = NULL;
SDL_Surface* sprite_surf = NULL;
SDL_Surface* font = NULL;

u8 CELL_SIZE = 8;

float PIXEL_SIZE = 6.5;
float TEXT_SCALING = 0.5;


float ANIM_SPEED = 0.008;
float PLAYER_SPEED = 0.1;
float JUMP_SPEED = -0.7;
float JUMP_DES = 0.002;

float FLOOR = 120;

char DEBUG_TEXT[200];
char FPS_TEXT[50];

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

float min(float a, float b){
    if(a > b) return b;
    return a;
}

float max(float a, float b){
    if(a > b) return a;
    return b;
}

typedef enum {
    Undefined = 0,
}TileType;

//TODO finish converting platform to tilemap
typedef struct Tile {
    i32 xpos;
    i32 ypos;

    int active;
    TileType type;
    i8 layer;

}Tile;

Tile GameMap[MAP_HEIGHT * MAP_WIDTH];


void add_platform(i32 xpos, i32 ypos){
    GameMap[xpos + (ypos * MAP_WIDTH)].active = 1;
}


void start_GameMap() {
    printf("Creating GameMap\n");

    size_t size = sizeof(GameMap)/sizeof(GameMap[0]);
    for(size_t i = 0; i < size-1; i++) GameMap[i].active = 0;


    add_platform(0, 4);
    add_platform(1, 4);
    add_platform(2, 5);
    add_platform(3, 4);
    //add_platform(2, 5, 150);
    add_platform(5, 8);
    add_platform(6, 7);
    add_platform(7, 6);
    add_platform(8, 5);

    //add_platform(6, 3);
    //add_platform(7, 4);

}

void draw_GameMap() {

    size_t size = sizeof(GameMap)/sizeof(GameMap[0]);
    for(size_t i = 0; i < size; i++) {

        if(!GameMap[i].active) continue;

        i32 final_size = (PIXEL_SIZE * CELL_SIZE);

        SDL_Rect src = {.x = (7) * CELL_SIZE, .y = (2) * CELL_SIZE, .w = CELL_SIZE, .h = CELL_SIZE};
        SDL_Rect dst = {.x = (i%MAP_WIDTH) * final_size, .y = (i/MAP_WIDTH) * final_size, .w = final_size, .h = final_size};

        SDL_BlitScaled(sprite_surf, &src, screen_surf, &dst);
    }
}

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

float get_floor(float xpos, float ypos) {

    // Player's minimum bouding box
    u32 min_x = (u32)((xpos + (PIXEL_SIZE * (1))) / (PIXEL_SIZE * CELL_SIZE));
    // Player's maximum bouding box
    u32 max_x = (u32)((xpos + (PIXEL_SIZE * (CELL_SIZE-2))) / (PIXEL_SIZE * CELL_SIZE));

    u32 y = (u32)(ypos / (PIXEL_SIZE * CELL_SIZE));

    float floor = 520;

    for(u32 i = y-1; i < MAP_HEIGHT - 2; i++){

        if(GameMap[min_x + ((i+1) * MAP_WIDTH)].active) {
            sprintf(DEBUG_TEXT,"%d\n", i);
            floor =  min((i * (PIXEL_SIZE * CELL_SIZE)), floor);
        }

        if(GameMap[max_x + ((i+1) * MAP_WIDTH)].active) {
            sprintf(DEBUG_TEXT,"%d\n", i);
            floor =  min((i * (PIXEL_SIZE * CELL_SIZE)), floor);
        }
    }


    return floor;
}

u32 anim_frame(Player* p) {
    return (((u32)p->anim_frame) % anim_duration(p->state)) + p->state;
}

void draw_player(Player* player){
    u32 frame = anim_frame(player);

    SDL_Rect src = {.x = (frame) * CELL_SIZE, .y = 0, .w = CELL_SIZE, .h = CELL_SIZE};
    SDL_Rect dst = {.x = (int)player->xpos, .y = (int)player->ypos, .w = (PIXEL_SIZE * CELL_SIZE), .h = (PIXEL_SIZE * CELL_SIZE)};

    SDL_BlitScaled(sprite_surf, &src, screen_surf, &dst);
}

void player_fall(Player* p, float elapsed){

    //printf("%0.2f %0.2f", p->ypos, p->yspeed);
    if(p->yspeed > 0) {
        p->state = FALL;
    } else {
        p->state = JUMPING;
    }

        if((p->ypos + p->yspeed) > get_floor(p->xpos, p->ypos)){
        p->yspeed = 0;
        p->ypos = get_floor(p->xpos, p->ypos);

        if(p->xspeed != 0) {
            p->state = WALING;
        } else {
            p->state = IDLE;
        }

        return;
    }


    p->yspeed += JUMP_DES * elapsed;
    p->ypos += p->yspeed * elapsed;
}

void update_player(Player* p, float elapsed) {
    //sprintf(DEBUG_TEXT, "A:%d;B:%d", (int)(p->xpos / (PIXEL_SIZE * CELL_SIZE) ), (int)p->ypos);

    p->anim_frame += elapsed * ANIM_SPEED;
    //p->xpos += p->xspeed * elapsed;
    float delta_xpos = p->xspeed * elapsed;

    if(p->ypos < get_floor(p->xpos + delta_xpos, p->ypos)) {
        p->state = FALL;
        p->xpos += delta_xpos;
    } else if(p->ypos > get_floor(p->xpos + delta_xpos, p->ypos)){
    } else {
        p->xpos += delta_xpos;
    }

    switch(p->state){
    case FALL: player_fall(p, elapsed); break;
    case JUMPING: player_fall(p, elapsed); break;
    default: break;
    }

}

void player_keydown(Player *p, SDL_Keycode key) {
    switch (p->state){

    case IDLE: switch(key){
        case SDLK_d: p->xspeed = PLAYER_SPEED;p->state = WALING; break;
        case SDLK_a: p->xspeed = -PLAYER_SPEED;p->state = WALING; break;
        case SDLK_w: p->yspeed = JUMP_SPEED;p->state = JUMPING; break;

        //Debug key
        //case SDLK_SPACE: sprintf(DEBUG_TEXT, "%d\n", (int)p->xpos); break;

        default: break;
        }; break;

    case WALING: switch(key){
        case SDLK_w: p->yspeed = JUMP_SPEED;p->state = JUMPING; break;

        default: break;
        }; break;

    default: switch(key) {
        case SDLK_d: p->xspeed = PLAYER_SPEED;break;
        case SDLK_a: p->xspeed = -PLAYER_SPEED;break;
        default: break;
        };break;
    }
}

void player_keyup(Player *p, SDL_Keycode key) {
    switch (p->state){

    case WALING: switch(key){
        case SDLK_d: p->xspeed = min(p->xspeed, 0); if(p->xspeed == 0) p->state = IDLE; break;
        case SDLK_a: p->xspeed = max(p->xspeed, 0); if(p->xspeed == 0) p->state = IDLE; break;

        default: break;
        }; break;


    default: switch(key){
        case SDLK_d: p->xspeed = min(p->xspeed, 0); break;
        case SDLK_a: p->xspeed = max(p->xspeed, 0); break;
        default: break;
        };break;
    }
}

void draw_debug_text(i32 x, i32 y, u32 color, char* text) {
    SDL_Surface* txt_surface = render_text(text, color);

    // Check if null
    if(!txt_surface) return;

    SDL_Rect dst = {.x = x, .y = y, .w = txt_surface->w * (int)(PIXEL_SIZE * TEXT_SCALING),   .h = txt_surface->h * (int)(PIXEL_SIZE * TEXT_SCALING)};
    SDL_BlitScaled(txt_surface, NULL, screen_surf, &dst);
}


int check(void* data, char* message) {
    if(data == NULL){
        printf("%s: ", message);
        printf("(%s)\n", SDL_GetError());

        return -1;
    }

    return 0;
}

void close_game() {

    SDL_DestroyWindow(window);
    window = NULL;

    printf("Program finished\n\n");

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

typedef struct FPSTimer {
    u32 reference_time;
    u32 to_count;
    u32 counted;
}FPSTimer;

void update_timer(FPSTimer* timer) {
    if(timer->counted <= timer->to_count) {
        timer->counted += 1;
        return;
    }

    timer->counted = 1;
    timer->reference_time = SDL_GetTicks();
}

float timer_fps(FPSTimer* timer) {
    // ms -> s
    float secs = ((float)(SDL_GetTicks() - timer->reference_time)) / 1000.0f;
    return ((float)timer->counted) / secs;
}

float timer_elapsed(FPSTimer* timer) {
    float ms = ((float)(SDL_GetTicks() - timer->reference_time));

    return ms / ((float)timer->counted);
}

int main(int argc, char** argv) {

    if(init("CatFormer", 800, 600) < 0) {
        printf("Coudn't init\n");
        close_game();
        return 0;
    }


    start_GameMap();
    load_sprite("/home/gabriel/Programming/Games/Assets/0x72_8x8TilesetF24.v1.bmp");

    int quit = 0;
    SDL_Event e;

    Player player = {.anim_frame = 0, .xpos = 0, .ypos = FLOOR, .state = IDLE};
    FPSTimer timer = {.reference_time = SDL_GetTicks(), .to_count = 1000, .counted = 0};


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

        //TODO fps capping
        update_timer(&timer);

        u32 fps = (u32)timer_fps(&timer);
        float elapsed = timer_elapsed(&timer);

        update_player(&player, elapsed);

        draw_GameMap();
        draw_player(&player);

        sprintf(FPS_TEXT, "%d", fps);
        draw_debug_text(5, 5, 0x00ff0040, FPS_TEXT);
        draw_debug_text(player.xpos - 20, player.ypos - 20, 0x00ffffa0, DEBUG_TEXT);

        // Swap buffers
        SDL_UpdateWindowSurface(window);
    }

    close_game();

    return 0;
}
