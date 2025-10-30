#include "image_processing.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ImageGray *load_image_gray(const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) { fprintf(stderr,"Erreur : impossible de charger %s\n", path); return NULL; }

    ImageGray *img = malloc(sizeof(ImageGray));
    img->w = surf->w;
    img->h = surf->h;
    img->pixels = malloc(img->w * img->h);

    Uint32 *data = surf->pixels;
    SDL_PixelFormat *fmt = surf->format;
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(data[y*img->w + x], fmt, &r, &g, &b);
            img->pixels[y*img->w + x] = (Uint8)((r + g + b)/3);
        }
    }
    SDL_FreeSurface(surf);
    return img;
}

void free_image_gray(ImageGray *img) {
    if (!img) return;
    free(img->pixels);
    free(img);
}

static void enhance_contrast(ImageGray *img) {
    int min = 255, max = 0;
    int size = img->w * img->h;
    for (int i = 0; i < size; i++) {
        if (img->pixels[i] < min) min = img->pixels[i];
        if (img->pixels[i] > max) max = img->pixels[i];
    }
    if (max-min < 10) return;
    for (int i = 0; i < size; i++) {
        img->pixels[i] = (Uint8)((img->pixels[i]-min)*255/(max-min));
    }
}

static void binarize_image_adaptive(ImageGray *img, int window) {
    int w = img->w, h = img->h;
    Uint8 *tmp = malloc(w*h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum=0,count=0;
            for(int dy=-window; dy<=window; dy++)
                for(int dx=-window; dx<=window; dx++){
                    int nx=x+dx, ny=y+dy;
                    if(nx>=0 && nx<w && ny>=0 && ny<h){
                        sum+=img->pixels[ny*w+nx];
                        count++;
                    }
                }
            tmp[y*w+x] = (img->pixels[y*w+x] < (sum/count)) ? 0 : 255;
        }
    }
    memcpy(img->pixels, tmp, w*h);
    free(tmp);
}

int preprocess_image(ImageGray *img){
    if(!img) return -1;
    enhance_contrast(img);
    binarize_image_adaptive(img,3);
    return 0;
}

