#include <3ds.h>
#include <stdio.h>
void event_FocusCamera(u32* args){
    printf("Focus on: %ld\n", args[0]);
}
