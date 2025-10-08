#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H


#include <SDL2/SDL.h>


typedef struct {
int w;
int h;
unsigned char *gray; /* grayscale bytes row-major */
unsigned char *bin; /* binarized image (0 or 255) */
} ImageGray;


ImageGray *load_image_gray(const char *path);
void free_image_gray(ImageGray *img);
int otsu_binarize(ImageGray *img);
void save_surface_png(SDL_Surface *surf, const char *path);


#endif
