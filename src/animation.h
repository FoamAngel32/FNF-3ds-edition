#ifndef ANIMATION_H
#define ANIMATION_H
#include <3ds.h>

typedef struct
{
    float frame;
    float frame_speed;
    float delay;
    float delay_speed;
    float max_delay;
    u32 loop_point;
    u32 total_frame;
    bool enabled;
} Animation;

u32 play_animation(Animation *animation);

#endif
