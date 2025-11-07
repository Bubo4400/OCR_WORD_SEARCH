#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>
#include <stdint.h>

// Structure qui représente une image en niveaux de gris
typedef struct {
    int w;        // largeur
    int h;        // hauteur
    Uint8 *pixels; // tableau de pixels (1 octet par pixel : 0..255)
} ImageGray;

// Protótypes des fonctions publiques
ImageGray *load_image_gray(const char *path);
void free_image_gray(ImageGray *img);
int preprocess_image(ImageGray *img);

#endif

