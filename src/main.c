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
#include "note.h"
#include "vorbis.h"
typedef uint64_t u64; ///< 64-bit unsigned integer
#define NOTE_WINDOW 150
#define JUDGMENT_LINE_Y (240 - 40)
#define STRUMLINE_START_X (60)
#define STRUMLINE_INTERVAL (65)
#define SPLASH_ANIM_SPEED 1
const char chart_header[] = "EMBEDDEDFNFCHART";
const char metadata_header[] = "EMBEDDEDFNFMETAD";
const char characters_header[] = "EMBEDDEDFNFCHARA";
const char stage_header[] = "EMBEDDEDFNFSTAGE";
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
C2D_Image imgs_note[30];
Note *notes2 = NULL;
// float notes[1024][2];
// char note_types[1024][2];
float scrollSpeed;
unsigned char game_paused;
u32 note_count;
u16 freeplay_song_rating;
u16 note_hitted;
u8 end;
u8 state;
touchPosition touch;
u32 keyHeld, keyDown, keyUp;
u32 temp1;
u8 counter_end;
double time_ms;
u8 music_hit_req_0;
u8 music_hit_req_1;
u8 music_hit_req_2;
u8 music_hit_req_3;
bool note_splash[4];
bool is_new_3ds;
u8 note_splash_frame[4][2];
// void load_song_meta_data(char *restrict song_path);
bool load_song(char *restrict song_path);
bool held_area(int x, int y, int width, int height);
bool touch_area(int x, int y, int width, int height);
bool release_area(int x, int y, int width, int height);
void draw_long_note(int id, float note_y, u8 a);
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
    int temp;
    for(temp = 0;temp < 23;++temp)
    {
        oggs[temp].s_audioBuffer = NULL;
    }
    // consoleInit(GFX_TOP, NULL);
    create_audio_thread();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    // staticTextBuf = C2D_TextBufNew(256);
    // printer_text_buf = C2D_TextBufNew(256);
    // top_target = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom_target = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    sprite_sheets[0] = C2D_SpriteSheetLoad("romfs:/gfx/bottom_bg.t3x");
    sprite_sheets[1] = C2D_SpriteSheetLoad("romfs:/gfx/strumlinenote.t3x");
    sprite_sheets[2] = C2D_SpriteSheetLoad("romfs:/gfx/note.t3x");
    imgs_note[0] = C2D_SpriteSheetGetImage(sprite_sheets[0], 0);

    imgs_note[1] = C2D_SpriteSheetGetImage(sprite_sheets[1], 0);
    imgs_note[2] = C2D_SpriteSheetGetImage(sprite_sheets[1], 1);
    imgs_note[3] = C2D_SpriteSheetGetImage(sprite_sheets[1], 2);
    imgs_note[4] = C2D_SpriteSheetGetImage(sprite_sheets[1], 3);
    imgs_note[5] = C2D_SpriteSheetGetImage(sprite_sheets[1], 4);

    imgs_note[6] = C2D_SpriteSheetGetImage(sprite_sheets[2], 0);
    imgs_note[7] = C2D_SpriteSheetGetImage(sprite_sheets[2], 1);
    imgs_note[8] = C2D_SpriteSheetGetImage(sprite_sheets[2], 2);
    imgs_note[9] = C2D_SpriteSheetGetImage(sprite_sheets[2], 3);

    imgs_note[10] = C2D_SpriteSheetGetImage(sprite_sheets[1], 5);
    imgs_note[11] = C2D_SpriteSheetGetImage(sprite_sheets[1], 6);
    imgs_note[12] = C2D_SpriteSheetGetImage(sprite_sheets[1], 7);

    imgs_note[13] = C2D_SpriteSheetGetImage(sprite_sheets[1], 8);
    imgs_note[14] = C2D_SpriteSheetGetImage(sprite_sheets[1], 9);
    imgs_note[15] = C2D_SpriteSheetGetImage(sprite_sheets[1], 10);

    imgs_note[16] = C2D_SpriteSheetGetImage(sprite_sheets[1], 11);
    imgs_note[17] = C2D_SpriteSheetGetImage(sprite_sheets[1], 12);
    imgs_note[18] = C2D_SpriteSheetGetImage(sprite_sheets[1], 13);

    imgs_note[19] = C2D_SpriteSheetGetImage(sprite_sheets[1], 14);
    imgs_note[20] = C2D_SpriteSheetGetImage(sprite_sheets[1], 15);
    imgs_note[21] = C2D_SpriteSheetGetImage(sprite_sheets[1], 16);

    imgs_note[22] = C2D_SpriteSheetGetImage(sprite_sheets[2], 4);
    imgs_note[23] = C2D_SpriteSheetGetImage(sprite_sheets[2], 5);

    imgs_note[24] = C2D_SpriteSheetGetImage(sprite_sheets[2], 6);
    imgs_note[25] = C2D_SpriteSheetGetImage(sprite_sheets[2], 7);

    imgs_note[26] = C2D_SpriteSheetGetImage(sprite_sheets[2], 8);
    imgs_note[27] = C2D_SpriteSheetGetImage(sprite_sheets[2], 9);

    imgs_note[28] = C2D_SpriteSheetGetImage(sprite_sheets[2], 10);
    imgs_note[29] = C2D_SpriteSheetGetImage(sprite_sheets[2], 11);

    consoleInit(GFX_TOP, NULL);
    // touched = false;
    load_vorbis("romfs:/music/songs/darnell.ogg", 0);
    // load_song_meta_data("romfs:/data/tutorial/metadata.bin");
    load_song("romfs:/data/songs/hard/darnell/chart.bin");
    // printf("%f, %d, %f\n", scrollSpeed, note_count, notes[0][0]);
    // printf("%d ", R_SUCCEEDED(result));
    // printf("%d\n", is_new_3ds);
    play_vorbis(0, false, 0, 1.0);
    u64 last_tick = svcGetSystemTick();
    const double ticks_per_ms = SYSCLOCK_ARM11 / 1000.0;
    note_hitted = 0;
    while (aptMainLoop() && !end)
    {
        hidScanInput();
        hidTouchRead(&touch);
        keyDown = hidKeysDown();
        keyHeld = hidKeysHeld();
        keyUp = hidKeysUp();
        u64 current_tick = svcGetSystemTick();
        double elapsed = (current_tick - last_tick) / ticks_per_ms;
        // if(((u64)time_ms % 1000) == 0)
        // printf("%f", time_ms);
        if (elapsed > 0)
        {
            if (!game_paused)
                time_ms += elapsed;
            last_tick = current_tick;
        }
        if (keyDown & KEY_START)
        {
            game_paused = !game_paused;
            pause_vorbis(0, game_paused);
        }
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        // C2D_SceneBegin(top_target);
        // C2D_TargetClear(top_target, C2D_Color32f(0, 0, 0, 1));
        C2D_SceneBegin(bottom_target);
        C2D_TargetClear(bottom_target, C2D_Color32f(0, 0, 0, 1));
        C2D_DrawImageAt(imgs_note[0], 0, 0, 0, NULL, 1.0f, 1.0f);
        int temp0;
        if (keyHeld & KEY_LEFT || held_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
        {
            if (music_hit_req_0 == 255)
            {
                note_splash_frame[0][1] = 0;
                music_hit_req_0 = 1;
                note_splash[0] = true;
            }
            if (!note_splash[0])
            {
                music_hit_req_0 = 1;
            }
            if (note_splash[0])
            {
                if (note_splash_frame[0][0] < SPLASH_ANIM_SPEED)
                    ++note_splash_frame[0][0];
                else
                {
                    note_splash_frame[0][0] = 0;
                    ++note_splash_frame[0][1];
                    if (note_splash_frame[0][1] > 2)
                        note_splash_frame[0][1] = 2;
                }
                C2D_DrawImageAtRotated(imgs_note[10 + note_splash_frame[0][1]], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
            }
            else
                C2D_DrawImageAtRotated(imgs_note[2], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        }
        else
            C2D_DrawImageAtRotated(imgs_note[1], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, (90 * M_PI / 180.0f), NULL, 1.0f, 1.0f);
        if (keyHeld & KEY_DOWN || held_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
        {
            if (music_hit_req_1 == 255)
            {
                note_splash_frame[1][1] = 0;
                music_hit_req_1 = 1;
                note_splash[1] = true;
            }
            if (!note_splash[1])
            {
                music_hit_req_1 = 2;
            }

            if (note_splash[1])
            {
                if (note_splash_frame[1][0] < SPLASH_ANIM_SPEED)
                    ++note_splash_frame[1][0];
                else
                {
                    note_splash_frame[1][0] = 0;
                    ++note_splash_frame[1][1];
                    if (note_splash_frame[1][1] > 2)
                        note_splash_frame[1][1] = 2;
                }
                C2D_DrawImageAtRotated(imgs_note[13 + note_splash_frame[1][1]], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
            }
            else
                C2D_DrawImageAtRotated(imgs_note[3], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        }
        else
            C2D_DrawImageAtRotated(imgs_note[1], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        if (keyHeld & KEY_UP || held_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
        {
            if (music_hit_req_2 == 255)
            {
                note_splash_frame[2][1] = 0;
                music_hit_req_2 = 1;
                note_splash[2] = true;
            }
            if (!note_splash[2])
            {
                music_hit_req_2 = 3;
            }
            if (note_splash[2])
            {
                if (note_splash_frame[2][0] < SPLASH_ANIM_SPEED)
                    ++note_splash_frame[2][0];
                else
                {
                    note_splash_frame[2][0] = 0;
                    ++note_splash_frame[2][1];
                    if (note_splash_frame[2][1] > 2)
                        note_splash_frame[2][1] = 2;
                }
                C2D_DrawImageAtRotated(imgs_note[16 + note_splash_frame[2][1]], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
            }
            else
                C2D_DrawImageAtRotated(imgs_note[4], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        }
        else
            C2D_DrawImageAtRotated(imgs_note[1], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, (180 * M_PI / 180.0f), NULL, 1.0f, 1.0f);
        if (keyHeld & KEY_RIGHT || held_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
        {
            if (music_hit_req_3 == 255)
            {
                note_splash_frame[3][1] = 0;
                music_hit_req_3 = 1;
                note_splash[3] = true;
            }
            if (!note_splash[3])
            {
                music_hit_req_3 = 4;
            }
            if (note_splash[3])
            {
                if (note_splash_frame[3][0] < SPLASH_ANIM_SPEED)
                    ++note_splash_frame[3][0];
                else
                {
                    note_splash_frame[3][0] = 0;
                    ++note_splash_frame[3][1];
                    if (note_splash_frame[3][1] > 2)
                        note_splash_frame[3][1] = 2;
                }
                C2D_DrawImageAtRotated(imgs_note[19 + note_splash_frame[3][1]], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
            }
            else
                C2D_DrawImageAtRotated(imgs_note[5], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        }
        else
            C2D_DrawImageAtRotated(imgs_note[1], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, (-90 * M_PI / 180.0f), NULL, 1.0f, 1.0f);
        for (temp0 = 0; temp0 < note_count; ++temp0)
        {
            float note_y = JUDGMENT_LINE_Y - (notes2[temp0].start - time_ms) * scrollSpeed + 19;
            float note_end_y = JUDGMENT_LINE_Y - ((notes2[temp0].start + notes2[temp0].length) - time_ms) * scrollSpeed + 19;
            if ((note_y < -50) || (note_end_y > 300))
                continue;

            if (((time_ms < (notes2[temp0].start + NOTE_WINDOW))) && (time_ms > (notes2[temp0].start - NOTE_WINDOW)))
            {
                if ((notes2[temp0].flag & 0b00000001) == 0)
                {
                    if (notes2[temp0].type == 0)
                    {
                        if (keyDown & KEY_LEFT || touch_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            notes2[temp0].flag |= 0b1;
                            notes2[temp0].flag |= 0b00000100;
                        }
                    }
                    else if (notes2[temp0].type == 1)
                    {
                        if (keyDown & KEY_DOWN || touch_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            notes2[temp0].flag |= 0b1;
                            notes2[temp0].flag |= 0b00000100;
                        }
                    }
                    else if (notes2[temp0].type == 2)
                    {
                        if (keyDown & KEY_UP || touch_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            notes2[temp0].flag |= 0b1;
                            notes2[temp0].flag |= 0b00000100;
                        }
                    }
                    else if (notes2[temp0].type == 3)
                    {
                        if (keyDown & KEY_RIGHT || touch_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            notes2[temp0].flag |= 0b1;
                            notes2[temp0].flag |= 0b00000100;
                        }
                    }
                }
                if (((notes2[temp0].flag & 0b00000010) == 0) && (notes2[temp0].flag & 0b00000001) != 0)
                {
                    if (notes2[temp0].type == 0)
                    {
                        if (keyHeld & KEY_LEFT || held_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            if (music_hit_req_0)
                            {
                                note_splash[0] = true;
                                if (notes2[temp0].length > 0)
                                {
                                    music_hit_req_0 = 255;
                                    notes2[temp0].length -= elapsed;
                                    notes2[temp0].start += elapsed;
                                }
                                else
                                {
                                    music_hit_req_0 = 0;
                                    notes2[temp0].flag |= 0b10;
                                }
                            }
                        }
                    }
                    else if (notes2[temp0].type == 1)
                    {
                        if (keyHeld & KEY_DOWN || held_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            if (music_hit_req_1)
                            {
                                note_splash[1] = true;
                                if (notes2[temp0].length > 0)
                                {
                                    music_hit_req_1 = 255;
                                    notes2[temp0].length -= elapsed;
                                    notes2[temp0].start += elapsed;
                                }
                                else
                                {
                                    music_hit_req_1 = 0;
                                    notes2[temp0].flag |= 0b10;
                                }
                            }
                        }
                    }
                    else if (notes2[temp0].type == 2)
                    {
                        // printf("UP KEY!");
                        if (keyHeld & KEY_UP || held_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            if (music_hit_req_2)
                            {
                                note_splash[2] = true;
                                if (notes2[temp0].length > 0)
                                {
                                    music_hit_req_2 = 255;
                                    notes2[temp0].length -= elapsed;
                                    notes2[temp0].start += elapsed;
                                }
                                else
                                {
                                    music_hit_req_2 = 0;
                                    notes2[temp0].flag |= 0b10;
                                }
                            }
                        }
                    }
                    else if (notes2[temp0].type == 3)
                    {
                        if (keyHeld & KEY_RIGHT || held_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                        {
                            if (music_hit_req_3)
                            {
                                note_splash[3] = true;
                                if (notes2[temp0].length > 0)
                                {
                                    music_hit_req_3 = 255;
                                    notes2[temp0].length -= elapsed;
                                    notes2[temp0].start += elapsed;
                                }
                                else
                                {
                                    music_hit_req_3 = 0;
                                    notes2[temp0].flag |= 0b10;
                                }
                            }
                        }
                    }
                }
            }

            if (notes2[temp0].length > 0)
                draw_long_note(temp0, note_y, notes2[temp0].type);
            if (notes2[temp0].flag & 0b00000100)
                continue;
            if (notes2[temp0].type == 0)
            {
                // printf("LEFT KEY!");
                C2D_DrawImageAt(imgs_note[6], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (notes2[temp0].type == 1)
            {
                // printf("DOWN KEY!");
                C2D_DrawImageAt(imgs_note[7], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (notes2[temp0].type == 2)
            {
                // printf("UP KEY!");
                C2D_DrawImageAt(imgs_note[8], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (notes2[temp0].type == 3)
            {
                // printf("RIGHT KEY!");
                C2D_DrawImageAt(imgs_note[9], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
        }
        C3D_FrameEnd(0);
        if (music_hit_req_0 == 1)
        {
            note_splash[0] = false;
            note_splash_frame[0][0] = 0;
            note_splash_frame[0][1] = 0;
            music_hit_req_0 = 0;
        }
        if (music_hit_req_1 == 1)
        {
            note_splash[1] = false;
            note_splash_frame[1][0] = 0;
            note_splash_frame[1][1] = 0;
            music_hit_req_1 = 0;
        }
        if (music_hit_req_2 == 1)
        {
            note_splash[2] = false;
            note_splash_frame[2][0] = 0;
            note_splash_frame[2][1] = 0;
            music_hit_req_2 = 0;
        }
        if (music_hit_req_3 == 1)
        {
            note_splash[3] = false;
            note_splash_frame[3][0] = 0;
            note_splash_frame[3][1] = 0;
            music_hit_req_3 = 0;
        }
        if (keyUp & KEY_LEFT)
        {
            note_splash[0] = false;
            note_splash_frame[0][0] = 0;
            note_splash_frame[0][1] = 0;
        }
        if (keyUp & KEY_DOWN)
        {
            note_splash[1] = false;
            note_splash_frame[1][0] = 0;
            note_splash_frame[1][1] = 0;
        }
        if (keyUp & KEY_UP)
        {
            note_splash[2] = false;
            note_splash_frame[2][0] = 0;
            note_splash_frame[2][1] = 0;
        }
        if (keyUp & KEY_RIGHT)
        {
            note_splash[3] = false;
            note_splash_frame[3][0] = 0;
            note_splash_frame[3][1] = 0;
        }
        if (keyUp & KEY_TOUCH)
        {
            memset(note_splash_frame, 0, sizeof(note_splash_frame));
            memset(note_splash, 0, sizeof(note_splash));
        }
        gspWaitForVBlank();
    }
    counter_end = true;
    free(notes2);
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
void draw_long_note(int id, float note_y, u8 a)
{
    float note_end_y = JUDGMENT_LINE_Y - ((notes2[id].start + notes2[id].length) - time_ms) * scrollSpeed;
    float scale = (note_y - note_end_y - 20) / 27.0f; 
    // note_end_y += 20;
    float long_note_x = (STRUMLINE_START_X + a * STRUMLINE_INTERVAL - 23) + 16;
    C2D_DrawImageAt(imgs_note[22 + a * 2], long_note_x, note_end_y + 20, 0, NULL, 1.0f, scale);
    // current_y -= 20;
    C2D_DrawImageAt(imgs_note[23 + a * 2], long_note_x, note_end_y, 0, NULL, 1.0f, -1.0f);
}
bool held_area(int x, int y, int width, int height)
{
    if (keyHeld & KEY_TOUCH)
    {
        if (((touch.px > x) && (touch.px < (x + width))) && ((touch.py > y) && (touch.py < (y + height))))
            return true;
    }
    return false;
}
bool touch_area(int x, int y, int width, int height)
{
    if (keyDown & KEY_TOUCH)
    {
        if (((touch.px > x) && (touch.px < (x + width))) && ((touch.py > y) && (touch.py < (y + height))))
            return true;
    }
    return false;
}
bool release_area(int x, int y, int width, int height)
{
    if (keyUp & KEY_TOUCH)
    {
        if (((touch.px > x) && (touch.px < (x + width))) && ((touch.py > y) && (touch.py < (y + height))))
            return true;
    }
    return false;
}

// void load_song_meta_data(char *restrict song_path)
// {
//     FILE *fh = fopen(song_path, "rb");
//     // for(temp1=0;temp1<)
//     size_t i = 0;
//     for (temp1 = 0; temp1 < 8; ++temp1)
//         if (fgetc(fh) == meta_data_header[temp1])
//             continue;
//         else
//             return;
//     int c;
//     while ((c = fgetc(fh)) != EOF && c != '\0' && i < sizeof(freeplay_song_week_name) - 1)
//     {
//         freeplay_song_week_name[i++] = (char)c;
//     }
//     freeplay_song_week_name[i] = '\0';
//     i = 0;
//     while ((c = fgetc(fh)) != EOF && c != '\0' && i < sizeof(freeplay_song_albumid) - 1)
//     {
//         freeplay_song_albumid[i++] = (char)c;
//     }
//     freeplay_song_albumid[i] = '\0';
//     freeplay_song_allow_erect = fgetc(fh);
//     u16 rating = 0;
//     int low = fgetc(fh);
//     int high = fgetc(fh);
//     freeplay_song_rating = (u16)(low | (high << 8));
//     fclose(fh);
// }
bool load_song(char *restrict song_path)
{
    if (notes2 != NULL)
    {
        free(notes2);
        notes2 = NULL; // Set to NULL immediately to prevent double-free crashes
    }
    u8 buffer[9];
    unsigned char raw_float[4];
    int temp0;
    char magic[17];
    FILE *fh = fopen(song_path, "rb");
    magic[16] = 0;
    fread(magic, 1, 16, fh);
    if (memcmp(magic, chart_header, 16) != 0)
    {
        return false;
    }
    fread(raw_float, 1, 4, fh);
    memcpy(&scrollSpeed, raw_float, sizeof(scrollSpeed));
    scrollSpeed /= 5;
    fread(buffer, 1, 4, fh);
    note_count = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    notes2 = (Note *)malloc(note_count * sizeof(Note));
    if (notes2 == NULL)
    {
        fclose(fh);
        return false;
    }
    for (temp0 = 0; temp0 < note_count; ++temp0)
    {
        fread(buffer, 1, 9, fh);
        memcpy(&notes2[temp0].start, &buffer[0], sizeof(float));
        memcpy(&notes2[temp0].length, &buffer[4], sizeof(float));
        notes2[temp0].type = buffer[8];
        notes2[temp0].flag = 0;
    }
    fclose(fh);
    return true;
}

// void thread_ms_counter(void *const unused)
// {

//     while (!counter_end)
//     {

//     }

//     // if (keyUp & KEY_LEFT)
//     // {
//     //     note_splash[0] = false;
//     //     note_splash_frame[0][0] = note_splash_frame[0][1] = 0;
//     // }
//     // if (keyUp & KEY_DOWN)
//     // {
//     //     note_splash[1] = false;
//     //     note_splash_frame[1][0] = note_splash_frame[1][1] = 0;
//     // }
//     // if (keyUp & KEY_UP)
//     // {
//     //     note_splash[2] = false;
//     //     note_splash_frame[2][0] = note_splash_frame[2][1] = 0;
//     // }
//     // if (keyUp & KEY_RIGHT)
//     // {
//     //     note_splash[3] = false;
//     //     note_splash_frame[3][0] = note_splash_frame[3][1] = 0;
//     // }
//     // if (keyUp & KEY_TOUCH)
//     // {
//     //     int temp0;
//     //     for (temp0 = 0; temp0 < 4; ++temp0)
//     //     {
//     //         note_splash[temp0] = false;
//     //         note_splash_frame[temp0][0] = note_splash_frame[temp0][1] = 0;
//     //     }
//     // }
// }
// void create_ms_counter_thread(void)
// {
//     // LightEvent_Init(&s_event2, RESET_ONESHOT);

//     // Set the thread priority to the main thread's priority ...
//     int32_t priority = 0x30;
//     svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
//     // ... then subtract 1, as lower number => higher actual priority ...
//     priority -= 1;
//     // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
//     priority = priority < 0x18 ? 0x18 : priority;
//     priority = priority > 0x3F ? 0x3F : priority;
//     counter_end = false;
//     // Start the thread, passing the address of our vorbisFile as an argument.
//     if (is_new_3ds)
//         threadId_ms_counter = threadCreate(thread_ms_counter, NULL,
//                                            THREAD_STACK_SZ, priority,
//                                            2, true);
//     else
//         threadId_ms_counter = threadCreate(thread_ms_counter, NULL,
//                                            THREAD_STACK_SZ, priority,
//                                            THREAD_AFFINITY, true);
// }
