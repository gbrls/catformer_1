#include <stdio.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/map.h"


//TODO oh shit this doesnt work
i32* parse_file(char* filename){

    FILE* f = fopen(filename, "r");

    // NULL if NULL
    if(!f) return NULL;

    u32 total_chars;
    char buffer[2<<16];

    for(total_chars = 0;;total_chars++){
        char c = fgetc(f);

        buffer[total_chars] = c;

        if(c == EOF) break;
    }

    i32 game_map[20 * 20];

    for(size_t i = 0; buffer[i] != EOF; i++) {
        int scanned;

        if(!sscanf(buffer + i, "%d", &scanned)) {
            printf("%d\n", scanned);
            game_map[i] = scanned;
        }
    }

    return 0;
}
