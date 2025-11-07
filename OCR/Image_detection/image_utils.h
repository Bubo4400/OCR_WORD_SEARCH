#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <SDL2/SDL.h>
#include "image_processing.h"

// Petite structure Rect utilisée pour draw_rect (différente de BoundingBox)
typedef struct {
    int x, y, w, h;
} Rect;

// fonctions pour l'image
ImageGray *crop_image(ImageGray *src, int x, int y, int w, int h);
void save_gray_bmp(ImageGray *img, const char *path);
void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color);
ImageGray *rotate_image(ImageGray *src, double angle_deg);
ImageGray *rotate_image_centered(ImageGray *src, double angle_deg);
ImageGray *rotate_image_expand(ImageGray *src, double angle_deg);

#endif

