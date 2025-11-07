#include "image_utils.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// découpe une sous-image (crop) depuis src aux coordonnées (x,y) de taille (w,h)
// et retourne une nouvelle ImageGray=> allouée
ImageGray *crop_image(ImageGray *src, int x, int y, int w, int h){
    if(!src) return NULL;
    ImageGray *crop = malloc(sizeof(ImageGray));
    if(!crop) return NULL;
    crop->w = w;
    crop->h = h;
    crop->pixels = malloc(w*h);
    if(!crop->pixels){ free(crop); return NULL; }

    // Copie pix par pix ; si la zone sort de l'image source, on met blanc (255)
    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            int src_x = x+i, src_y = y+j;
            if(src_x>=0 && src_x<src->w && src_y>=0 && src_y<src->h)
                crop->pixels[j*w+i] = src->pixels[src_y*src->w+src_x];
            else
                crop->pixels[j*w+i] = 255; // bord blanc
        }
    }
    return crop;
}

// Save une ImageGray en BMP
void save_gray_bmp(ImageGray *img, const char *path){
    if(!img) return;
    // Crée une surface 32-bit RGBA (SDL doit avoir ce format pour SaveBMP)
    SDL_Surface *surface = SDL_CreateRGBSurface(0,img->w,img->h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
    if(!surface) return;

    Uint32 *pixels = surface->pixels;
    // On copie chaque pixel gris en valeur RGB identique
    for(int y=0;y<img->h;y++){
        for(int x=0;x<img->w;x++){
            Uint8 v = img->pixels[y*img->w + x];
            pixels[y*img->w+x] = SDL_MapRGB(surface->format,v,v,v);
        }
    }
    // Save le BMP dans le path
    SDL_SaveBMP(surface,path);
    SDL_FreeSurface(surface);
}

// Dessiner un rectangle
void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color){
    SDL_SetRenderDrawColor(ren,color.r,color.g,color.b,color.a);
    SDL_Rect rect = {r.x,r.y,r.w,r.h};
    SDL_RenderDrawRect(ren,&rect);
}

// Rotation a amméliorer :rotation par nearest-neighbor
ImageGray *rotate_image(ImageGray *src, double angle_deg) {
  :  if (!src) return NULL;
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
            // Calcul de la source correspondante par rotation inverse (pr chaque pix, on regade sur l'image d'avant)
            int xr = (int)((x - cx) * cosA + (y - cy) * sinA + cx);
            int yr = (int)(-(x - cx) * sinA + (y - cy) * cosA + cy);
            if (xr >= 0 && xr < w && yr >= 0 && yr < h)
                rot->pixels[y * w + x] = src->pixels[yr * w + xr];
            else
                rot->pixels[y * w + x] = 255; // fond blanc si hors image
        }
    }
    return rot;
}

// Rotation centrée (v différente pour réorientation interactive)
ImageGray *rotate_image_centered(ImageGray *src, double angle_deg) {
    if (!src) return NULL;
    double angle = angle_deg * M_PI / 180.0;

    int w = src->w, h = src->h;
    int cx = w / 2, cy = h / 2;

    ImageGray *rot = malloc(sizeof(ImageGray));
    rot->w = w;
    rot->h = h;
    rot->pixels = malloc(w * h);
    // Initialise tout en blanc
    for (int i = 0; i < w * h; i++) rot->pixels[i] = 255;

    // Remplissage par backward mapping : pour chaque pixel cible, calcule la source
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int dx = x - cx;
            int dy = y - cy;
            int src_x = (int)( cos(-angle) * dx - sin(-angle) * dy + cx );
            int src_y = (int)( sin(-angle) * dx + cos(-angle) * dy + cy );
            if (src_x >= 0 && src_x < w && src_y >= 0 && src_y < h)
                rot->pixels[y*w + x] = src->pixels[src_y*w + src_x];
        }
    }
    return rot;
}

// Rotation qui agrandit la zone pour contenir l'image tournée 
ImageGray *rotate_image_expand(ImageGray *src, double angle_deg) {
    if (!src) return NULL;
    double angle = angle_deg * M_PI / 180.0;
    int w = src->w, h = src->h;

    // Taille du carré qui contient la diagonale
    int new_size = (int)(sqrt(w*w + h*h));
    int cx = new_size / 2, cy = new_size / 2;
    ImageGray *rot = malloc(sizeof(ImageGray));
    rot->w = new_size;
    rot->h = new_size;
    rot->pixels = malloc(new_size * new_size);
    for (int i = 0; i < new_size * new_size; i++) rot->pixels[i] = 255;

    // Backward mapping : pour chaque pixel on remonte a la pos d'origine av rotation sinon a a des trous
    for (int y = 0; y < new_size; y++) {
        for (int x = 0; x < new_size; x++) {
            int dx = x - cx;
            int dy = y - cy;
            int src_x = (int)( cos(-angle)*dx - sin(-angle)*dy + w/2 );
            int src_y = (int)( sin(-angle)*dx + cos(-angle)*dy + h/2 );
            if (src_x >= 0 && src_x < w && src_y >= 0 && src_y < h)
                rot->pixels[y*new_size + x] = src->pixels[src_y*w + src_x];
        }
    }

    return rot;
}


/*
 * Pour une rotation directe :x' = cosθ*(x - cx) - sinθ*(y - cy) + cx
y' = sinθ*(x - cx) + cosθ*(y - cy) + cy

Pour une rotation indirecte :src_x = cos(-θ)*(x - cx) - sin(-θ)*(y - cy) + cx
src_y = sin(-θ)*(x - cx) + cos(-θ)*(y - cy) + cy
*/
