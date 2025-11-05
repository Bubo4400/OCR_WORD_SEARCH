#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <SDL2/SDL.h>
#include "image_processing.h"

typedef struct {
    int x, y, w, h;
} Rect;

ImageGray *crop_image(ImageGray *src, int x, int y, int w, int h);
void save_gray_bmp(ImageGray *img, const char *path);
void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color);
ImageGray *rotate_image(ImageGray *src, double angle_deg);
#endif

