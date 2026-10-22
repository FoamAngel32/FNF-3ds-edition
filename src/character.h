#ifndef CHARACTER_H
#define CHARACTER_H
#include <3ds.h>
#include <citro2d.h>
#include "animation.h"
typedef struct
{
    u32 x, y;
    unsigned char character; //0-bf 1-opponent 2-gf
    u32 *frames;
    Animation current_animation;
    bool has_dance_anim;
} Character;
#endif