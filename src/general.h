#ifndef GENERAL_H
#define GENERAL_H
#include <citro2d.h>
enum GameStates {
    STATE_MENU = 0,
    STATE_SELECT = 1,
    STATE_PLAY = 2,
};
typedef struct{
    float start;
    float length;
    char type;
    char flag;
} Note;
typedef void (*StateFunction)(void);

#define NOTE_WINDOW 120
#define JUDGMENT_LINE_Y (240 - 40)
#define STRUMLINE_START_X (60)
#define STRUMLINE_INTERVAL (65)
#define SPLASH_ANIM_SPEED (2)
#define TOTAL_STATE (3)

extern Note *notes;
extern float scrollSpeed;
extern u32 note_count;
extern u32 keyHeld, keyDown, keyUp;
extern touchPosition touch;
extern C2D_SpriteSheet sprite_sheets[4];
extern int state;
extern C3D_RenderTarget *top_target, *bottom_target;
extern float scrollSpeed;

extern StateFunction func_state_init[TOTAL_STATE];
extern StateFunction func_state_update[TOTAL_STATE];

#endif