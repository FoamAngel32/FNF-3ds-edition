#ifndef GENERAL_H
#define GENERAL_H
#include <citro2d.h>
#include "song.h"
enum GameStates
{
    STATE_TITLE = 0,
    STATE_STORY_MODE = 1,
    STATE_FREEPLAY = 2,
    STATE_OPTIONS = 3,
    STATE_PLAY = 4,
};

typedef void (*StateFunction)(void);
typedef void (*EventTrigger)(u32* args);

#define NOTE_WINDOW 120
#define JUDGMENT_LINE_Y (240 - 40)
#define STRUMLINE_START_X (60)
#define STRUMLINE_INTERVAL (65)
#define SPLASH_ANIM_SPEED (2)
#define TOTAL_STATE (5)
#define TOTAL_EVENT_TYPES (1)


extern C3D_RenderTarget *top_target, *bottom_target;
extern C2D_SpriteSheet sprite_sheets[8];
extern C2D_Image images[128];
extern Note *notes;
extern Event *events;
extern u32 note_count;
extern u32 event_count;
extern float scrollSpeed;
extern u32 keyHeld, keyDown, keyUp;
extern touchPosition touch;
extern int state;
extern float bpm;

extern StateFunction func_state_init[TOTAL_STATE];
extern StateFunction func_state_update[TOTAL_STATE];
extern EventTrigger events_trigger[TOTAL_EVENT_TYPES];

#endif