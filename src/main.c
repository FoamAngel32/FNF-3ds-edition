#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
#include "states/play_state.h"
#include "general.h"
#include "vorbis.h"
#include "utils.h"
typedef uint64_t u64; ///< 64-bit unsigned integer
// song ids
// 0 main song
#define VORBIS_ID_SONG 0
// ---- DEFINITIONS ----

C3D_RenderTarget *top_target, *bottom_target;
// C3D_RenderTarget *bottom_target;
// C2D_SpriteSheet temp_sheet;
C2D_SpriteSheet sprite_sheets[4];
// C2D_Font fonts[2];
// static C2D_TextBuf staticTextBuf;
// static C2D_Text texts[9];
// static C2D_TextBuf printer_text_buf;
// static C2D_Text printer_text;
Note *notes = NULL;
u32 note_count;
float scrollSpeed;
float health;
u16 freeplay_song_rating;
u8 end;
int state;
touchPosition touch;
u32 keyHeld, keyDown, keyUp;
u32 temp1;
u8 counter_end;
bool is_new_3ds;
// void load_song_meta_data(char *restrict song_path);
int main(void)
{
    aptInit();
    hidInit();
    gfxInitDefault();
    romfsInit();
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_MONO);
// #define FORCE_SLOW_MODE
#ifndef FORCE_SLOW_MODE
    APT_CheckNew3DS(&is_new_3ds);
    if (is_new_3ds)
    {
        // PTMSYSM_ConfigureNew3DSCPU(0b11);
        osSetSpeedupEnable(true);
    }
#endif
    vorbis_init();
    // consoleInit(GFX_TOP, NULL);
    create_audio_thread();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    // staticTextBuf = C2D_TextBufNew(256);
    // printer_text_buf = C2D_TextBufNew(256);
    // top_target = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom_target = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);


    consoleInit(GFX_TOP, NULL);
    // touched = false;
    load_vorbis("romfs:/music/songs/darnell.ogg", 0);
    // load_song_meta_data("romfs:/data/tutorial/metadata.bin");
    if(!load_song("romfs:/data/songs/darnell/darnell-hard.bin"))
    {
        printf("ERROR!!");
        while (aptMainLoop())
            gspWaitForVBlank();
    }
    // printf("%f, %d, %f\n", scrollSpeed, note_count, notes[0][0]);
    // printf("%d ", R_SUCCEEDED(result));
    // printf("%d\n", is_new_3ds);
    func_state_init[STATE_PLAY]();
    while (aptMainLoop() && !end)
    {
        hidScanInput();
        hidTouchRead(&touch);
        keyDown = hidKeysDown();
        keyHeld = hidKeysHeld();
        keyUp = hidKeysUp();

        if(state < TOTAL_STATE)
            func_state_update[state]();
        gspWaitForVBlank();
    }
    counter_end = true;
    free(notes);
    aptExit();
    hidExit();
    audioExit();
    ndspExit();
    romfsExit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return EXIT_SUCCESS;
}
