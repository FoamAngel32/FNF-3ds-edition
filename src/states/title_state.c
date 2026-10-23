#include "../general.h"
void init_TitleState(void)
{
    int temp;
    for(temp = 0; temp < 8; ++temp)
        if(sprite_sheets[temp])
            C2D_SpriteSheetFree(sprite_sheets[temp]);
}

void update_TitleState(void)
{

}
