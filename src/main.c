#include <tremor/ivorbisfile.h>
#include <tremor/ivorbiscodec.h>
#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
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
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
OggVorbis_File vorbis_file[128];
u8 temp0_audio;
Thread threadId_audio;
Thread threadId_ms_counter;
typedef struct
{
    int vorbis_id;
    int chn;
    signed char done;
    ndspWaveBuf s_waveBufs[3];
    int16_t *s_audioBuffer;
    float pitch;
} OggVorbis;
OggVorbis oggs[23];
static const int THREAD_AFFINITY = -1;        // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024; // 32kB stack for audio thread

// ---- END DEFINITIONS ----

LightEvent s_event;
LightEvent s_event2;
volatile bool s_quit = false; // Quit flag

// Copied from audio example with a slight bit of change

// Pause until user presses a button

// ---- END HELPER FUNCTIONS ----

// Audio initialisation code
// This sets up NDSP and our primary audio buffer
bool audioInit(OggVorbis *vorbisFile_, int chn, int id, float pitch);
// Audio de-initialisation code
// Stops playback and frees the primary audio buffer
void audioExit(void);
bool fillBuffer(OggVorbis_File *vorbisFile_, ndspWaveBuf *waveBuf_, int chn);
// NDSP audio frame callback
// This signals the audioThread to decode more things
// once NDSP has played a sound frame, meaning that there should be
// one or more available waveBufs to fill with more data.
void audioCallback_ogg(void *const nul_);
// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void audioThread(void *const unused);
void stop_vorbis(int id);
void load_vorbis(char *restrict song_path, int chn);
void play_vorbis(int chn, bool loop, int id, float pitch);
void create_audio_thread(void);
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
float notes[1024][2];
char note_types[1024][2];
float scrollSpeed;
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
u8 thread_ms_req_0;
u8 thread_ms_req_1;
u8 thread_ms_req_2;
u8 thread_ms_req_3;
bool note_splash[4];
bool is_new_3ds;
bool count_start;
u8 note_splash_frame[4][2];
// void load_song_meta_data(char *restrict song_path);
bool load_song(char *restrict song_path);
bool held_area(int x, int y, int width, int height);
bool touch_area(int x, int y, int width, int height);
bool release_area(int x, int y, int width, int height);
void draw_long_note(int id, float note_y, u8 a);
int main(void)
{
    count_start = false;
    ptmSysmInit();
    PTMSYSM_CheckNew3DS(&is_new_3ds);
    ptmSysmExit();
    hidInit();
    gfxInitDefault();
    romfsInit();
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    // consoleInit(GFX_TOP, NULL);
    create_audio_thread();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    // #define FORCE_SLOW_MODE
    #ifndef FORCE_SLOW_MODE
    if (is_new_3ds)
    {
        // PTMSYSM_ConfigureNew3DSCPU(0b11);
        osSetSpeedupEnable(true);
    }
    #endif
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
    load_vorbis("romfs:/music/songs/tutorial.ogg", 0);
    // load_song_meta_data("romfs:/data/tutorial/metadata.bin");
    load_song("romfs:/data/songs/normal/tutorial/chart.bin");
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
            time_ms += elapsed;
            last_tick = current_tick;
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
            if (!note_splash[0])
            {
                thread_ms_req_0 = 1;
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
            if (!note_splash[1])
            {
                thread_ms_req_1 = 2;
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
            if (!note_splash[2])
            {
                thread_ms_req_2 = 3;
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
            if (!note_splash[3])
            {
                thread_ms_req_3 = 4;
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
            float note_y = JUDGMENT_LINE_Y - (notes[temp0][0] - time_ms) * scrollSpeed;
            float note_end_y = JUDGMENT_LINE_Y - ((notes[temp0][0]+notes[temp0][1]) - time_ms) * scrollSpeed;
            if ((note_y < -50) || (note_end_y > 300))
                continue;
            if (note_types[temp0][1])
            {
                continue;
            }
            if (notes[temp0][1] == 0)
            {
                if (((time_ms < (notes[temp0][0] + NOTE_WINDOW))) && (time_ms > (notes[temp0][0] - NOTE_WINDOW)))
                {
                    if ((note_types[temp0][1] & 0b00000001) == 0)
                    {
                        if (note_types[temp0][0] == 0)
                        {
                            if (keyDown & KEY_LEFT || touch_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                note_types[temp0][1] |= 0b1;
                            }
                        }
                        else if (note_types[temp0][0] == 1)
                        {
                            if (keyDown & KEY_DOWN || touch_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                note_types[temp0][1] |= 0b1;
                            }
                        }
                        else if (note_types[temp0][0] == 2)
                        {
                            if (keyDown & KEY_UP || touch_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                note_types[temp0][1] |= 0b1;
                            }
                        }
                        else if (note_types[temp0][0] == 3)
                        {
                            if (keyDown & KEY_RIGHT || touch_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                note_types[temp0][1] |= 0b1;
                            }
                        }
                    }
                    if (((note_types[temp0][1] & 0b00000010) == 0) && (note_types[temp0][1] & 0b00000001) != 0)
                    {
                        if (note_types[temp0][0] == 0)
                        {
                            if (keyHeld & KEY_LEFT || held_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                if (thread_ms_req_0)
                                {
                                    note_types[temp0][1] |= 0b10;
                                    note_splash[0] = true;
                                    thread_ms_req_0 = 0;
                                }
                            }
                        }
                        else if (note_types[temp0][0] == 1)
                        {
                            if (keyHeld & KEY_DOWN || held_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                if (thread_ms_req_1)
                                {
                                    note_types[temp0][1] |= 0b10;
                                    note_splash[1] = true;
                                    thread_ms_req_1 = 0;
                                }
                            }
                        }
                        else if (note_types[temp0][0] == 2)
                        {
                            // printf("UP KEY!");
                            if (keyHeld & KEY_UP || held_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                if (thread_ms_req_2)
                                {
                                    note_types[temp0][1] |= 0b10;
                                    note_splash[2] = true;
                                    thread_ms_req_2 = 0;
                                }
                            }
                        }
                        else if (note_types[temp0][0] == 3)
                        {
                            if (keyHeld & KEY_RIGHT || held_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                            {
                                if (thread_ms_req_3)
                                {
                                    note_types[temp0][1] |= 0b10;
                                    note_splash[3] = true;
                                    thread_ms_req_3 = 0;
                                }
                            }
                        }
                    }
                }
            }

            if (notes[temp0][1] > 0)
                draw_long_note(temp0, note_y, note_types[temp0][0]);
            if (note_types[temp0][0] == 0)
            {
                // printf("LEFT KEY!");
                C2D_DrawImageAt(imgs_note[6], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (note_types[temp0][0] == 1)
            {
                // printf("DOWN KEY!");
                C2D_DrawImageAt(imgs_note[7], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (note_types[temp0][0] == 2)
            {
                // printf("UP KEY!");
                C2D_DrawImageAt(imgs_note[8], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
            else if (note_types[temp0][0] == 3)
            {
                // printf("RIGHT KEY!");
                C2D_DrawImageAt(imgs_note[9], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
            }
        }
        C3D_FrameEnd(0);
        if (thread_ms_req_0)
        {
            note_splash[0] = false;
            note_splash_frame[0][0] = 0;
            note_splash_frame[0][1] = 0;
            thread_ms_req_0 = 0;
        }
        if (thread_ms_req_1)
        {
            note_splash[1] = false;
            note_splash_frame[1][0] = 0;
            note_splash_frame[1][1] = 0;
            thread_ms_req_1 = 0;
        }
        if (thread_ms_req_2)
        {
            note_splash[2] = false;
            note_splash_frame[2][0] = 0;
            note_splash_frame[2][1] = 0;
            thread_ms_req_2 = 0;
        }
        if (thread_ms_req_3)
        {
            note_splash[3] = false;
            note_splash_frame[3][0] = 0;
            note_splash_frame[3][1] = 0;
            thread_ms_req_3 = 0;
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
    float note_end_y = JUDGMENT_LINE_Y - ((notes[id][0] + notes[id][1]) - time_ms) * scrollSpeed;
    // note_end_y += 20;
    float long_note_x = (STRUMLINE_START_X + a * STRUMLINE_INTERVAL - 23) + 16;
    float current_y = note_y - 10;
    for (; current_y > note_end_y; current_y -= 20)
    {
        C2D_DrawImageAt(imgs_note[22 + a * 2], long_note_x, current_y, 0, NULL, 1.0f, -1.0f);
    }
    // current_y -= 20;
    C2D_DrawImageAt(imgs_note[23 + a * 2], long_note_x, current_y, 0, NULL, 1.0f, -1.0f);
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
    unsigned char raw_float[4];
    int temp0;
    char magic[17];
    unsigned char b0, b1, b2, b3;
    FILE *fh = fopen(song_path, "rb");
    magic[16] = 0;
    fread(magic, 1, 16, fh);
    if (memcmp(magic, chart_header, 16) != 0)
    {
        return false;
    }
    raw_float[0] = fgetc(fh);
    raw_float[1] = fgetc(fh);
    raw_float[2] = fgetc(fh);
    raw_float[3] = fgetc(fh);
    memcpy(&scrollSpeed, raw_float, sizeof(scrollSpeed));
    scrollSpeed /= 10;
    b0 = fgetc(fh);
    b1 = fgetc(fh);
    b2 = fgetc(fh);
    b3 = fgetc(fh);
    note_count = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    for (temp0 = 0; temp0 < note_count; ++temp0)
    {
        raw_float[0] = fgetc(fh);
        raw_float[1] = fgetc(fh);
        raw_float[2] = fgetc(fh);
        raw_float[3] = fgetc(fh);
        memcpy(&notes[temp0][0], raw_float, sizeof(notes[temp0][0]));
        raw_float[0] = fgetc(fh);
        raw_float[1] = fgetc(fh);
        raw_float[2] = fgetc(fh);
        raw_float[3] = fgetc(fh);
        memcpy(&notes[temp0][1], raw_float, sizeof(notes[temp0][1]));
        note_types[temp0][0] = fgetc(fh);
        note_types[temp0][1] = 0;
    }
    fclose(fh);
    return true;
}
bool audioInit(OggVorbis *vorbisFile_, int chn, int id, float pitch)
{
    ov_pcm_seek(&vorbis_file[id], 0);
    vorbis_info *vi = ov_info(&vorbis_file[vorbisFile_->vorbis_id], -1);
    vorbisFile_->done = 0;
    vorbisFile_->chn = chn;
    vorbisFile_->vorbis_id = id;
    vorbisFile_->pitch = pitch;
    // Setup NDSP
    ndspChnReset(chn);
    ndspChnSetInterp(chn, NDSP_INTERP_NONE);
    ndspChnSetRate(chn, 32000 * pitch);
    ndspChnSetFormat(chn, vi->channels == 1
                              ? NDSP_FORMAT_MONO_PCM16
                              : NDSP_FORMAT_STEREO_PCM16);

    // Allocate audio buffer
    // 120ms buffer
    const size_t SAMPLES_PER_BUF = vi->rate * 120 / 1000;
    // mono (1) or stereo (2)
    const size_t CHANNELS_PER_SAMPLE = vi->channels;
    // s16 buffer
    const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * sizeof(s16);
    const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(vorbisFile_->s_waveBufs);
    vorbisFile_->s_audioBuffer = (int16_t *)linearAlloc(bufferSize);
    // Setup waveBufs for NDSP
    memset(&vorbisFile_->s_waveBufs, 0, sizeof(vorbisFile_->s_waveBufs));
    int16_t *buffer = vorbisFile_->s_audioBuffer;

    for (size_t i = 0; i < ARRAY_SIZE(vorbisFile_->s_waveBufs); ++i)
    {
        vorbisFile_->s_waveBufs[i].data_vaddr = buffer;
        vorbisFile_->s_waveBufs[i].nsamples = WAVEBUF_SIZE / sizeof(buffer[0]);
        vorbisFile_->s_waveBufs[i].status = NDSP_WBUF_DONE;

        buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
    }

    return true;
}
void audioExit(void)
{
    for (temp0_audio = 0; temp0_audio <= 23; ++temp0_audio)
    {
        stop_vorbis(temp0_audio);
        ndspChnReset(temp0_audio);
    }
    for (temp0_audio = 0; temp0_audio < ARRAY_SIZE(oggs); ++temp0_audio)
    {
        ov_clear(&vorbis_file[oggs[temp0_audio].vorbis_id]);
        linearFree(oggs[temp0_audio].s_audioBuffer);
    }
}
bool fillBuffer(OggVorbis_File *vorbisFile_, ndspWaveBuf *waveBuf_, int chn)
{
    int totalBytes = 0;
    bool eof = false;

    // printf("FIlled buffer\n");
    while (totalBytes < waveBuf_->nsamples * sizeof(s16))
    {
        int16_t *buffer = waveBuf_->data_pcm16 + (totalBytes / sizeof(s16));
        size_t bufferSize = waveBuf_->nsamples * sizeof(s16) - totalBytes;

        int bytesRead = ov_read(vorbisFile_, (char *)buffer, bufferSize, NULL);
        if (bytesRead == 0)
        { // EOF
            eof = true;
            break;
        }
        if (bytesRead < 0)
        {
            eof = true;
            break;
        }

        totalBytes += bytesRead;
    }

    if (totalBytes == 0 && eof)
    {
        // printf("FIll buffer eof\n");
        // ov_pcm_seek();
        return false;
    }

    waveBuf_->nsamples = totalBytes / sizeof(s16);
    ndspChnWaveBufAdd(chn, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16, totalBytes);
    return true;
}
void stop_vorbis(int id)
{
    oggs[id].done = 1;
}
void load_vorbis(char *restrict song_path, int id)
{
    stop_vorbis(id);
    ov_raw_seek(&vorbis_file[id], 0);
    ov_clear(&vorbis_file[id]);
    FILE *fh = fopen(song_path, "rb");
    ov_open(fh, &vorbis_file[id], NULL, 0);
}
void audioCallback_ogg(void *const nul_)
{
    // printf("Audio Callback!\n");
    (void)nul_; // Unused
    // ov_raw_seek(&vorbis_file[0], 0);
    if (s_quit)
    { // Quit flag
        return;
    }

    LightEvent_Signal(&s_event);
}
void audioThread(void *const unused)
{
    while (!s_quit)
    { // Whilst the quit flag is unset,
      // search our waveBufs and fill any that aren't currently
      // queued for playback (i.e, those that are 'done')
        for (temp0_audio = 0; temp0_audio < ARRAY_SIZE(oggs); ++temp0_audio)
        {
            if (oggs[temp0_audio].done == 1)
                continue;
            // printf("doing for channel %d               \n", temp0_audio);
            for (size_t i = 0; i < ARRAY_SIZE(oggs[temp0_audio].s_waveBufs); ++i)
            {
                // printf("%d ", oggs[temp0_audio].s_waveBufs[i].status);
                if (oggs[temp0_audio].s_waveBufs[i].status != NDSP_WBUF_DONE)
                    continue;
                else if (oggs[temp0_audio].s_waveBufs[i].status == NDSP_WBUF_DONE)
                {
                    if (!fillBuffer(&vorbis_file[oggs[temp0_audio].vorbis_id], &oggs[temp0_audio].s_waveBufs[i], oggs[temp0_audio].chn))
                    { // Playback complete
                        // printf("");
                        if (oggs[temp0_audio].done == -1)
                        {
                            play_vorbis(temp0_audio, true, oggs[temp0_audio].vorbis_id, oggs[temp0_audio].pitch);
                            // audioInit(&oggs[temp0_audio], oggs[temp0_audio].chn, oggs[temp0_audio].vorbis_id, oggs[temp0_audio].pitch);
                        }
                        else if (oggs[temp0_audio].done == 0)
                            oggs[temp0_audio].done = 1;
                        break;
                    }
                }
                // else if(oggs[temp0_audio].s_waveBufs[i].status == NDSP_WBUF_DONE){
                //     memset(&oggs[temp0_audio].s_waveBufs[i], 0, sizeof(oggs[temp0_audio].s_waveBufs[i]));
                // }
            }
            // printf("\n");
        }
        // Wait for a signal that we're needed again before continuing,
        // so that we can yield to other things that want to run
        // (Note that the 3DS uses cooperative threading)
        LightEvent_Wait(&s_event);
    }
}
void play_vorbis(int chn, bool loop, int id, float pitch)
{
    // Attempt audioInit
    // if (audioInit(&oggs[chn], chn, id, pitch))
    audioInit(&oggs[chn], chn, id, pitch);
    if (loop)
        oggs[chn].done = -1;
    else
        oggs[chn].done = 0;
    // Set the ndsp sound frame callback which signals our audioThread
}
void create_audio_thread(void)
{
    LightEvent_Init(&s_event, RESET_ONESHOT);
    // Spawn audio thread

    // Set the thread priority to the main thread's priority ...
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    // ... then subtract 1, as lower number => higher actual priority ...
    priority -= 2;
    // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;

    // Start the thread, passing the address of our vorbisFile as an argument.
    ndspSetCallback(audioCallback_ogg, NULL);
    for (temp1 = 0; temp1 <= 23; ++temp1)
    {
        oggs[temp1].done = 1;
    }
    if (is_new_3ds)
    {
        threadId_audio = threadCreate(audioThread, NULL,
                                      THREAD_STACK_SZ, priority,
                                      2, true);
    }
    else
        threadId_audio = threadCreate(audioThread, NULL,
                                      THREAD_STACK_SZ, priority,
                                      THREAD_AFFINITY, true);
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
