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
        fprintf(stderr, "Erreur : impossible de charger %s\n", path);
        return NULL;
    }

    ImageGray *img = malloc(sizeof(ImageGray));
    img->w = surf->w;
    img->h = surf->h;
    img->pixels = malloc(img->w * img->h);

    Uint32 *data = surf->pixels;
    SDL_PixelFormat *fmt = surf->format;
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(data[y * img->w + x], fmt, &r, &g, &b);
            img->pixels[y * img->w + x] = (r + g + b) / 3;
        }
    }

    SDL_FreeSurface(surf);
    return img;
}

void free_image_gray(ImageGray *img)
{
    if (!img) return;
    free(img->pixels);
    free(img);
}

/*---------------------------------------------
 * Contraste automatique
 *--------------------------------------------*/
static void enhance_contrast(ImageGray *img)
{
    int min = 255, max = 0;
    int size = img->w * img->h;

    for (int i = 0; i < size; i++) {
        if (img->pixels[i] < min) min = img->pixels[i];
        if (img->pixels[i] > max) max = img->pixels[i];
    }
    if (max - min < 10) return;

    for (int i = 0; i < size; i++) {
        img->pixels[i] = (img->pixels[i] - min) * 255 / (max - min);
    }
}

/*---------------------------------------------
 * Binarisation simple
 *--------------------------------------------*/
static void binarize_image(ImageGray *img, int threshold)
{
    int size = img->w * img->h;
    for (int i = 0; i < size; i++) {
        img->pixels[i] = (img->pixels[i] < threshold) ? 0 : 255;
    }
}

/*---------------------------------------------
 * Pipeline de prétraitement complet
 *--------------------------------------------*/
int preprocess_image(ImageGray *img)
{
    if (!img) return -1;
    enhance_contrast(img);
    binarize_image(img, 128);
    return 0;
}

