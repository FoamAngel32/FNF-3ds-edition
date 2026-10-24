#ifndef SONG_H
#define SONG_H
typedef struct
{
    float start;
    float length;
    char type;
    char flag;
} Note;
typedef struct
{
    float time;
    u16 id;
    u32 args[4];
    bool triggered;
} Event;
#endif