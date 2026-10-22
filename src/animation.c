#include <3ds.h>
#include "animation.h"

u32 play_animation(Animation* animation)
{
    if(animation->enabled)
    {
        animation->delay += animation->delay_speed;
        if(animation->delay >= animation->max_delay)
        {
            animation->delay = 0;
            animation->frame += animation->frame_speed;
            if(animation->frame >= animation->total_frame)
                animation->frame = animation->loop_point;
        }
    }
    return (u32)animation->frame;
}
