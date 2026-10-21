#ifndef UTILS_H
#define UTILS_H
bool held_area(int x, int y, int width, int height);
bool touch_area(int x, int y, int width, int height);
bool release_area(int x, int y, int width, int height);
bool load_song(char *restrict song_path);
#endif