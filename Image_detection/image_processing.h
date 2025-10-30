#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct {
    int w;
    int h;
    Uint8 *pixels; // niveaux de gris
} ImageGray;

ImageGray *load_image_gray(const char *path);
void free_image_gray(ImageGray *img);
int preprocess_image(ImageGray *img); /* contraste + binarisation adaptative */

#endif

