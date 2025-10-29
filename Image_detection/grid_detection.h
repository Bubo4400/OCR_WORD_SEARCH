#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "contour_detection.h"

// Détecte la grille principale du mot caché
BoundingBox detect_main_grid(BoundingBox *boxes, int count);

// Découpe la grille en cases individuelles
void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols);

// Détection et extraction des mots à côté de la grille
void extract_side_words(ImageGray *img, BoundingBox grid);

#endif

