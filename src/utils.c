#include <3ds.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "general.h"
const char chart_header[] = "EMBEDDEDFNFCHART";
const char events_header[] = "EMBEDDEDFNFEVENT";
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
bool load_chart(char *restrict song_path)
{
    if (notes != NULL)
    {
        free(notes);
        notes = NULL;
    }
    u8 buffer[9];
    int temp0;
    char magic[16];
    FILE *fh = fopen(song_path, "rb");
    if (!fh)
        return false;
        
    fread(magic, 1, 16, fh);
    if (memcmp(magic, chart_header, 16) != 0)
    {
        fclose(fh);
        return false;
    }
    fread(&scrollSpeed, 1, 4, fh);
    scrollSpeed /= 5;
    fread(&note_count, 1, 4, fh);
    // note_count = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
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
    return true;
}

bool load_events(char *restrict song_path)
{
    if (events != NULL)
    {
        free(events);
        events = NULL;
    }
    char magic[16];
    int temp0;
    FILE *fh = fopen(song_path, "rb");
    if (!fh)
        return false;
        
    fread(magic, 1, 16, fh);
    if (memcmp(magic, events_header, 16) != 0)
    {
        fclose(fh);
        return false;
    }
    fread(&event_count, 1, 4, fh);
    events = (Event *)malloc(event_count * sizeof(Event));
    if (events == NULL)
    {
        fclose(fh);
        return false;
    }
    u16 event_type;
    float time;
    for (temp0 = 0; temp0 < event_count; ++temp0)
    {
        events[temp0].triggered = false;
        events[temp0].args[3] = events[temp0].args[2] = events[temp0].args[1] = events[temp0].args[0] = 0;
        fread(&time, 1, 4, fh);
        events[temp0].time = time;
        fread(&event_type, 1, 2, fh);
        events[temp0].id = event_type;
        if(event_type == 0)
            events[temp0].args[0] = fgetc(fh);
        // printf("Loading... %d\n", temp0);
    }
    fclose(fh);
    return true;
}
