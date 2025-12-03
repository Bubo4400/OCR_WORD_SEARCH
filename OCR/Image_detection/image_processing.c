#include "image_processing.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Charge une image depuis le chemin 'path' =>convertit en niveau de gris
ImageGray *load_image_gray(const char *path) {
    SDL_Surface *surf = IMG_Load(path); // bibl SDL_image pour charger PNG/JPG

    ImageGray *img = malloc(sizeof(ImageGray)); // alloue la structure
    img->w = surf->w;
    img->h = surf->h;
    img->pixels = malloc(img->w * img->h); // 1 octet par pixel (gris)

    //Récupération des pixels en RGB => conversion en gris
    Uint32 *data = surf->pixels;
    SDL_PixelFormat *fmt = surf->format;
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Uint8 r, g, b;
            //Récupère la couleur au format SDL 
            SDL_GetRGB(data[y*img->w + x], fmt, &r, &g, &b);
            // Conversion en une valeur de gris (moyenne de RGB)
            img->pixels[y*img->w + x] = (Uint8)((r + g + b)/3);
        }
    }
    SDL_FreeSurface(surf); // on libère la surface SDL
    return img; // retourne l'image en niveaux de gris
}

// Free
void free_image_gray(ImageGray *img) {
    if (!img) return;
    free(img->pixels);
    free(img);
}

// améliore le contraste 
static void enhance_contrast(ImageGray *img) {
    int min = 255, max = 0;
    int size = img->w * img->h;
    // recherche du min et max dans l'image
    for (int i = 0; i < size; i++) {
        if (img->pixels[i] < min) min = img->pixels[i];
        if (img->pixels[i] > max) max = img->pixels[i];
    }
    // si pas trop de contraste, on ne fait rien ( et évite division par zéro, pas a refaire)
    if (max-min < 10) return;
    // etalement linéaire : nouvelle_valeur = (val-min)*255/(max-min)
    for (int i = 0; i < size; i++) {
        img->pixels[i] = (Uint8)((img->pixels[i]-min)*255/(max-min));
    }
}

// Binarisation: pour chaque pixel on compare avec la moyenne locale
// car lumière peut changer noir et que le seuil global ne fonctionnerait pas assez bien
static void binarize_image_adaptive(ImageGray *img, int window) {
    int w = img->w, h = img->h;
    Uint8 *tmp = malloc(w*h); // buffer tmp pour stocker le résultat
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum=0,count=0;
            // Moyenne sur une fenêtre carrée centrée sur (x,y)
            for(int dy=-window; dy<=window; dy++)
                for(int dx=-window; dx<=window; dx++){
                    int nx=x+dx, ny=y+dy;
                    if(nx>=0 && nx<w && ny>=0 && ny<h){
                        sum+=img->pixels[ny*w+nx];
                        count++;
                    }
                }
            // Si le pixel est inférieur à la moyenne locale -> noir(0), sinon blanc(255)
            tmp[y*w+x] = (img->pixels[y*w+x] < (sum/count)) ? 0 : 255;
        }
    }
    // On copie le résultat dans l'image d'origine
    memcpy(img->pixels, tmp, w*h);
    free(tmp);
}

// effectue le prétraitement sur une image (contraste + binarisation)
int preprocess_image(ImageGray *img){
    if(!img) return -1;
    enhance_contrast(img);
    binarize_image_adaptive(img,3); // fenêtre = 3 pixels
    return 0;
}

