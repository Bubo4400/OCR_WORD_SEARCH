#include "image_processing.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*---------------------------------------------
 * Conversion, libération, et utilitaires
 *--------------------------------------------*/
ImageGray *load_image_gray(const char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "Error loading image %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_LockSurface(surf);
    ImageGray *img = malloc(sizeof(ImageGray));
    img->w = surf->w;
    img->h = surf->h;
    img->gray = malloc(img->w * img->h);
    img->bin  = malloc(img->w * img->h);

    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)surf->pixels;
    for (int y = 0; y < surf->h; y++) {
        for (int x = 0; x < surf->w; x++) {
            SDL_GetRGB(pixels[y * surf->w + x], surf->format, &r, &g, &b);
            unsigned char gray_val = (Uint8)(0.299*r + 0.587*g + 0.114*b);
            img->gray[y * surf->w + x] = gray_val;
        }
    }

    SDL_UnlockSurface(surf);
    SDL_FreeSurface(surf);
    return img;
}

void free_image_gray(ImageGray *img)
{
    if (!img) return;
    free(img->gray);
    free(img->bin);
    free(img);
}

/*---------------------------------------------
 * Contraste automatique
 *--------------------------------------------*/
static void enhance_contrast(ImageGray *img)
{
    int min = 255, max = 0;
    for (int i = 0; i < img->w * img->h; i++) {
        if (img->gray[i] < min) min = img->gray[i];
        if (img->gray[i] > max) max = img->gray[i];
    }
    if (max - min < 10) return;

    for (int i = 0; i < img->w * img->h; i++)
        img->gray[i] = (img->gray[i] - min) * 255 / (max - min);
}

/*---------------------------------------------
 * Binarisation adaptative
 *--------------------------------------------*/
static void adaptive_binarize(ImageGray *img, int window_size, int offset)
{
    int w = img->w, h = img->h;
    int half = window_size / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0, count = 0;
            for (int j = -half; j <= half; j++) {
                for (int i = -half; i <= half; i++) {
                    int nx = x + i, ny = y + j;
                    if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                        sum += img->gray[ny * w + nx];
                        count++;
                    }
                }
            }
            int mean = sum / count;
            int val = img->gray[y * w + x];
            img->bin[y * w + x] = (val < mean - offset) ? 0 : 255;
        }
    }
}

/*---------------------------------------------
 * Pipeline de prétraitement complet
 *--------------------------------------------*/
int preprocess_image(ImageGray *img)
{
    if (!img) return -1;
    enhance_contrast(img);
    adaptive_binarize(img, 15, 10);
    return 0;
}

