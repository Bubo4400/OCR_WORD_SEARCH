#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <SDL2/SDL.h>

typedef struct { int x, y, w, h; } Rect;

SDL_Surface *load_image_grayscale(const char *path);
void threshold_image(SDL_Surface *surf, Uint8 thresh);
void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color);

#endif

