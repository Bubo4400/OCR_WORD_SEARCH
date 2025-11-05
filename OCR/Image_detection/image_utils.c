#include "image_utils.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
ImageGray *crop_image(ImageGray *src, int x, int y, int w, int h){
    if(!src) return NULL;
    ImageGray *crop = malloc(sizeof(ImageGray));
    if(!crop) return NULL;
    crop->w = w;
    crop->h = h;
    crop->pixels = malloc(w*h);
    if(!crop->pixels){ free(crop); return NULL; }

    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            int src_x = x+i, src_y = y+j;
            if(src_x>=0 && src_x<src->w && src_y>=0 && src_y<src->h)
                crop->pixels[j*w+i] = src->pixels[src_y*src->w+src_x];
            else
                crop->pixels[j*w+i] = 255;
        }
    }
    return crop;
}

void save_gray_bmp(ImageGray *img, const char *path){
    if(!img) return;
    SDL_Surface *surface = SDL_CreateRGBSurface(0,img->w,img->h,32,
        0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
    if(!surface) return;

    Uint32 *pixels = surface->pixels;
    for(int y=0;y<img->h;y++){
        for(int x=0;x<img->w;x++){
            Uint8 v = img->pixels[y*img->w + x];
            pixels[y*img->w+x] = SDL_MapRGB(surface->format,v,v,v);
        }
    }
    SDL_SaveBMP(surface,path);
    SDL_FreeSurface(surface);
}

void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color){
    SDL_SetRenderDrawColor(ren,color.r,color.g,color.b,color.a);
    SDL_Rect rect = {r.x,r.y,r.w,r.h};
    SDL_RenderDrawRect(ren,&rect);
}

ImageGray *rotate_image(ImageGray *src, double angle_deg) {
    if (!src) return NULL;
    double angle = angle_deg * M_PI / 180.0;
    double cosA = cos(angle), sinA = sin(angle);
    int w = src->w, h = src->h;

    ImageGray *rot = malloc(sizeof(ImageGray));
    rot->w = w;
    rot->h = h;
    rot->pixels = malloc(w * h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int cx = w / 2, cy = h / 2;
            int xr = (int)((x - cx) * cosA + (y - cy) * sinA + cx);
            int yr = (int)(-(x - cx) * sinA + (y - cy) * cosA + cy);
            if (xr >= 0 && xr < w && yr >= 0 && yr < h)
                rot->pixels[y * w + x] = src->pixels[yr * w + xr];
            else
                rot->pixels[y * w + x] = 255;
        }
    }
    return rot;
}

