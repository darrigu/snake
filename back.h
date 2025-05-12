#ifndef BACK_H
#define BACK_H

#define FACTOR 70
#define WIDTH (16*FACTOR)
#define HEIGHT (9*FACTOR)

typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s32;
typedef int b32;
typedef float f32;

typedef enum {
  KEY_LEFT,
  KEY_RIGHT,
  KEY_UP,
  KEY_DOWN,
} Key;

void front_fill_rect(s32 x, s32 y, s32 w, s32 h, u32 color);
void front_panic(const char* file_path, s32 line, const char* message);
void front_log(const char* message);
s32 front_keydown(Key key);

#endif // BACK_H
