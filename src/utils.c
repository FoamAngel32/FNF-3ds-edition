#include <3ds.h>
#include <stdio.h>
#include <malloc.h>
#include "general.h"
const char chart_header[] = "EMBEDDEDFNFCHART";
const char metadata_header[] = "EMBEDDEDFNFMETAD";
const char characters_header[] = "EMBEDDEDFNFCHARA";
const char stage_header[] = "EMBEDDEDFNFSTAGE";

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
bool load_song(char *restrict song_path)
{
    if (notes != NULL)
    {
        free(notes);
        notes = NULL; // Set to NULL immediately to prevent double-free crashes
    }
    u8 buffer[9];
    unsigned char raw_float[4];
    int temp0;
    char magic[17];
    FILE *fh = fopen(song_path, "rb");
    if (!fh)
        return false;
    magic[16] = 0;
    fread(magic, 1, 16, fh);
    if (memcmp(magic, chart_header, 16) != 0)
    {
        fclose(fh);
        return false;
    }
    fread(raw_float, 1, 4, fh);
    memcpy(&scrollSpeed, raw_float, sizeof(scrollSpeed));
    scrollSpeed /= 5;
    fread(buffer, 1, 4, fh);
    note_count = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    notes = (Note *)malloc(note_count * sizeof(Note));
    // printf("%d\n", sizeof(Note));
    if (notes == NULL)
    {
        fclose(fh);
        return false;
    }
    for (temp0 = 0; temp0 < note_count; ++temp0)
    {
        fread(buffer, 1, 9, fh);
        memcpy(&notes[temp0].start, &buffer[0], sizeof(float));
        memcpy(&notes[temp0].length, &buffer[4], sizeof(float));
        notes[temp0].type = buffer[8];
        notes[temp0].flag = 0;
        // printf("Loading... %d\n", temp0);
    }
    fclose(fh);
    gspWaitForVBlank();
    return true;
}
