#ifndef CONTOUR_DETECTION_H
#define CONTOUR_DETECTION_H

#include "image_processing.h"

// Structure qui représente une boîte englobante (x,y,width,height) qui va permettre de trouver les lettres
typedef struct {
    int x, y, w, h;
} BoundingBox;

// Trouve les contours (zones sombres) dans l'image binaire.
// out_boxes : adresse d'un pointeur qui recevra un tableau alloué de BoundingBox
// count : adresse où sera écrit le nombre de boîtes trouvées
// La fonction alloue de la mémoire pour out_boxes ; le main doit free() après usage sinon memory leak
int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count);

// Dessine les contours pour vérifier
void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count);

#endif

