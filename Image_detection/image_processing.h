#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>
#include <stdint.h>

// Structure de base pour les images en niveaux de gris
typedef struct {
    int w;
    int h;
    Uint8 *pixels;  // tableau de niveaux de gris (0–255)
} ImageGray;

// Fonctions de gestion
ImageGray *load_image_gray(const char *path);
void free_image_gray(ImageGray *img);

// Fonctions de prétraitement
int preprocess_image(ImageGray *img);

#endif

