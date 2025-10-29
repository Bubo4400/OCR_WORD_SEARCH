#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>

typedef struct {
    int w, h;
    unsigned char *gray;
    unsigned char *bin;
} ImageGray;

ImageGray *load_image_gray(const char *path);
void free_image_gray(ImageGray *img);
int preprocess_image(ImageGray *img);

#endif

