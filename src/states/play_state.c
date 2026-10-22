#include <citro2d.h>
#include "../general.h"
#include "../utils.h"
#include "../vorbis.h"
#include "../animation.h"
const double ticks_per_ms = SYSCLOCK_ARM11 / 1000.0;
unsigned char game_paused;
double time_ms;
u8 music_hit_req_0;
u8 music_hit_req_1;
u8 music_hit_req_2;
u8 music_hit_req_3;
u64 last_tick;
float health;
float display_health;
bool note_splash[4];
C2D_Image imgs_note[26];
Animation anim_note_splash[4];
C2D_ImageTint bottom_bg_tint;
void draw_long_note(int id, float note_y, u8 a);
void init_PlayState(void)
{
    state = STATE_PLAY;
    time_ms = 0;
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

    imgs_note[18] = C2D_SpriteSheetGetImage(sprite_sheets[2], 4);
    imgs_note[19] = C2D_SpriteSheetGetImage(sprite_sheets[2], 5);

    imgs_note[20] = C2D_SpriteSheetGetImage(sprite_sheets[2], 6);
    imgs_note[21] = C2D_SpriteSheetGetImage(sprite_sheets[2], 7);

    imgs_note[22] = C2D_SpriteSheetGetImage(sprite_sheets[2], 8);
    imgs_note[23] = C2D_SpriteSheetGetImage(sprite_sheets[2], 9);

    imgs_note[24] = C2D_SpriteSheetGetImage(sprite_sheets[2], 10);
    imgs_note[25] = C2D_SpriteSheetGetImage(sprite_sheets[2], 11);

   
    C2D_PlainImageTint(&bottom_bg_tint, C2D_Color32(49, 38, 51, 255), 1.0f);

    int temp;
    for (temp = 0; temp < 4; ++temp)
    {
        anim_note_splash[temp].max_delay = anim_note_splash[temp].total_frame = anim_note_splash[temp].loop_point = 1;
        anim_note_splash[temp].frame_speed = 24.0f / 60.0f;
        anim_note_splash[temp].delay_speed = 1;
    }

    play_vorbis(0, false, 0, 1.0);
    last_tick = svcGetSystemTick();
}
void update_PlayState(void)
{
    u64 current_tick = svcGetSystemTick();
    double elapsed = (current_tick - last_tick) / ticks_per_ms;
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
    display_health += (health - display_health) / 6.0f;
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // C2D_SceneBegin(top_target);
    // C2D_TargetClear(top_target, C2D_Color32f(0, 0, 0, 1));
    C2D_SceneBegin(bottom_target);
    C2D_TargetClear(bottom_target, C2D_Color32(62, 48, 64, 255));
    C2D_DrawImageAt(imgs_note[0], 0, 0, 0, &bottom_bg_tint, 1.0f, 1.0f);
    // 
    int temp0;
    if (keyHeld & KEY_LEFT || held_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
    {
        if (music_hit_req_0 == 255)
        {
            anim_note_splash[0].enabled = false;
            anim_note_splash[0].frame = 0;
            anim_note_splash[0].delay = 0;
            music_hit_req_0 = 1;
            note_splash[0] = true;
        }
        if (!note_splash[0])
        {
            music_hit_req_0 = 1;
        }
        if (note_splash[0])
        {
            anim_note_splash[0].enabled = true;
            C2D_DrawImageAtRotated(imgs_note[10 + play_animation(&anim_note_splash[0])], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
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
            anim_note_splash[1].enabled = false;
            anim_note_splash[1].frame = 0;
            anim_note_splash[1].delay = 0;
            music_hit_req_1 = 1;
            note_splash[1] = true;
        }
        if (!note_splash[1])
        {
            music_hit_req_1 = 2;
        }

        if (note_splash[1])
        {
            anim_note_splash[1].enabled = true;
            C2D_DrawImageAtRotated(imgs_note[12 + play_animation(&anim_note_splash[1])], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
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
            anim_note_splash[2].enabled = false;
            anim_note_splash[2].frame = 0;
            anim_note_splash[2].delay = 0;
            music_hit_req_2 = 1;
            note_splash[2] = true;
        }
        if (!note_splash[2])
        {
            music_hit_req_2 = 3;
        }
        if (note_splash[2])
        {
            anim_note_splash[2].enabled = true;
            C2D_DrawImageAtRotated(imgs_note[14 + play_animation(&anim_note_splash[2])], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
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
            anim_note_splash[3].enabled = false;
            anim_note_splash[3].frame = 0;
            anim_note_splash[3].delay = 0;
            music_hit_req_3 = 1;
            note_splash[3] = true;
        }
        if (!note_splash[3])
        {
            music_hit_req_3 = 4;
        }
        if (note_splash[3])
        {
            anim_note_splash[3].enabled = true;
            C2D_DrawImageAtRotated(imgs_note[16 + play_animation(&anim_note_splash[3])], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
        }
        else
            C2D_DrawImageAtRotated(imgs_note[5], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, 0, NULL, 1.0f, 1.0f);
    }
    else
        C2D_DrawImageAtRotated(imgs_note[1], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL, JUDGMENT_LINE_Y, 0, (-90 * M_PI / 180.0f), NULL, 1.0f, 1.0f);
    for (temp0 = 0; temp0 < note_count; ++temp0)
    {
        float note_y = JUDGMENT_LINE_Y - (notes[temp0].start - time_ms) * scrollSpeed + 10;
        float note_end_y = JUDGMENT_LINE_Y - ((notes[temp0].start + notes[temp0].length) - time_ms) * scrollSpeed + 10;
        if ((note_y < -50) || (note_end_y > 300))
            continue;

        if (((time_ms < (notes[temp0].start + NOTE_WINDOW))) && (time_ms > (notes[temp0].start - NOTE_WINDOW)))
        {
            if ((notes[temp0].flag & 0b00000001) == 0)
            {
                if (notes[temp0].type == 0)
                {
                    if (keyDown & KEY_LEFT || touch_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        notes[temp0].flag |= 0b1;
                        notes[temp0].flag |= 0b00000100;
                    }
                }
                else if (notes[temp0].type == 1)
                {
                    if (keyDown & KEY_DOWN || touch_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        notes[temp0].flag |= 0b1;
                        notes[temp0].flag |= 0b00000100;
                    }
                }
                else if (notes[temp0].type == 2)
                {
                    if (keyDown & KEY_UP || touch_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        notes[temp0].flag |= 0b1;
                        notes[temp0].flag |= 0b00000100;
                    }
                }
                else if (notes[temp0].type == 3)
                {
                    if (keyDown & KEY_RIGHT || touch_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        notes[temp0].flag |= 0b1;
                        notes[temp0].flag |= 0b00000100;
                    }
                }
            }
            if (((notes[temp0].flag & 0b00000010) == 0) && (notes[temp0].flag & 0b00000001) != 0)
            {
                if (notes[temp0].type == 0)
                {
                    if (keyHeld & KEY_LEFT || held_area(STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        if (music_hit_req_0)
                        {
                            note_splash[0] = true;
                            if (notes[temp0].length > 0)
                            {
                                music_hit_req_0 = 255;
                                notes[temp0].length -= elapsed;
                                notes[temp0].start += elapsed;
                            }
                            else
                            {
                                music_hit_req_0 = 0;
                                notes[temp0].flag |= 0b10;
                            }
                        }
                    }
                }
                else if (notes[temp0].type == 1 && !note_splash[1])
                {
                    if (keyHeld & KEY_DOWN || held_area(STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        if (music_hit_req_1)
                        {
                            note_splash[1] = true;
                            if (notes[temp0].length > 0)
                            {
                                music_hit_req_1 = 255;
                                notes[temp0].length -= elapsed;
                                notes[temp0].start += elapsed;
                            }
                            else
                            {
                                music_hit_req_1 = 0;
                                notes[temp0].flag |= 0b10;
                            }
                        }
                    }
                }
                else if (notes[temp0].type == 2 && !note_splash[2])
                {
                    // printf("UP KEY!");
                    if (keyHeld & KEY_UP || held_area(STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        if (music_hit_req_2)
                        {
                            note_splash[2] = true;
                            if (notes[temp0].length > 0)
                            {
                                music_hit_req_2 = 255;
                                notes[temp0].length -= elapsed;
                                notes[temp0].start += elapsed;
                            }
                            else
                            {
                                music_hit_req_2 = 0;
                                notes[temp0].flag |= 0b10;
                            }
                        }
                    }
                }
                else if (notes[temp0].type == 3 && !note_splash[3])
                {
                    if (keyHeld & KEY_RIGHT || held_area(STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 23, JUDGMENT_LINE_Y - 23, 46, 46))
                    {
                        if (music_hit_req_3)
                        {
                            note_splash[3] = true;
                            if (notes[temp0].length > 0)
                            {
                                music_hit_req_3 = 255;
                                notes[temp0].length -= elapsed;
                                notes[temp0].start += elapsed;
                            }
                            else
                            {
                                music_hit_req_3 = 0;
                                notes[temp0].flag |= 0b10;
                            }
                        }
                    }
                }
            }
        }

        if (notes[temp0].type < 4)
            if (notes[temp0].length > 0)
                draw_long_note(temp0, note_y, notes[temp0].type);
        if (notes[temp0].flag & 0b00000100)
            continue;
        if (notes[temp0].type == 0)
        {
            // printf("LEFT KEY!");
            C2D_DrawImageAt(imgs_note[6], STRUMLINE_START_X + 0 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
        }
        else if (notes[temp0].type == 1)
        {
            // printf("DOWN KEY!");
            C2D_DrawImageAt(imgs_note[7], STRUMLINE_START_X + 1 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
        }
        else if (notes[temp0].type == 2)
        {
            // printf("UP KEY!");
            C2D_DrawImageAt(imgs_note[8], STRUMLINE_START_X + 2 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
        }
        else if (notes[temp0].type == 3)
        {
            // printf("RIGHT KEY!");
            C2D_DrawImageAt(imgs_note[9], STRUMLINE_START_X + 3 * STRUMLINE_INTERVAL - 32, note_y - 32, 0, NULL, 1.0f, 1.0f);
        }
    }
    C3D_FrameEnd(0);
    if (music_hit_req_0 == 1)
    {
        note_splash[0] = false;
        anim_note_splash[0].frame = 0;
        anim_note_splash[0].delay = 0;
        music_hit_req_0 = 0;
    }
    if (music_hit_req_1 == 1)
    {
        note_splash[1] = false;
        anim_note_splash[1].frame = 0;
        anim_note_splash[1].delay = 0;
        music_hit_req_1 = 0;
    }
    if (music_hit_req_2 == 1)
    {
        note_splash[2] = false;
        anim_note_splash[2].frame = 0;
        anim_note_splash[2].delay = 0;
        music_hit_req_2 = 0;
    }
    if (music_hit_req_3 == 1)
    {
        note_splash[3] = false;
        anim_note_splash[3].frame = 0;
        anim_note_splash[3].delay = 0;
        music_hit_req_3 = 0;
    }
    if (keyUp & KEY_LEFT)
    {
        note_splash[0] = false;
        anim_note_splash[0].frame = 0;
        anim_note_splash[0].delay = 0;
    }
    if (keyUp & KEY_DOWN)
    {
        note_splash[1] = false;
        anim_note_splash[1].frame = 0;
        anim_note_splash[1].delay = 0;
    }
    if (keyUp & KEY_UP)
    {
        note_splash[2] = false;
        anim_note_splash[2].frame = 0;
        anim_note_splash[2].delay = 0;
    }
    if (keyUp & KEY_RIGHT)
    {
        note_splash[3] = false;
        anim_note_splash[3].frame = 0;
        anim_note_splash[3].delay = 0;
    }
    if (keyUp & KEY_TOUCH)
    {
        int temp = 0;
        for(;temp < 4;++temp)
        {
            anim_note_splash[temp].frame = 0;
            anim_note_splash[temp].delay = 0;
        }
        memset(note_splash, 0, sizeof(note_splash));
    }
}
void draw_long_note(int id, float note_y, u8 a)
{
    float note_end_y = JUDGMENT_LINE_Y - ((notes[id].start + notes[id].length) - time_ms) * scrollSpeed;
    float scale = (note_y - note_end_y - 20) / 27.0f;
    // note_end_y += 20;
    float long_note_x = (STRUMLINE_START_X + a * STRUMLINE_INTERVAL - 23) + 16;
    C2D_DrawImageAt(imgs_note[18 + a * 2], long_note_x, note_end_y + 20, 0, NULL, 1.0f, scale);
    // current_y -= 20;
    C2D_DrawImageAt(imgs_note[19 + a * 2], long_note_x, note_end_y, 0, NULL, 1.0f, -1.0f);
}