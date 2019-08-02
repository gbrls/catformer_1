#include <stdio.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/map.h"

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




    printf("The file (%s) contains %d characters\n", buffer, total_chars);

    return 0;
}
