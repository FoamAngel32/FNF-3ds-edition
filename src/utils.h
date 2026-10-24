#ifndef UTILS_H
#define UTILS_H
#include "general.h"

bool held_area(int x, int y, int width, int height);
bool touch_area(int x, int y, int width, int height);
bool release_area(int x, int y, int width, int height);
bool load_chart(char *restrict song_path);
bool load_events(char *restrict song_path);

#endif