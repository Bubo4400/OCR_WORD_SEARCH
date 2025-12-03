#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "contour_detection.h"
#include "image_processing.h"

// Détecte automatiquement le zones "mot" et "grille"
// word_zone et grid_zone sont des pointeurs vers des BoundingBox vont etre remplis
void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone);

// Extrait et sauvegarde les lettres trouvées deux zones
void extract_letters_from_zones(ImageGray *img, BoundingBox word_zone, BoundingBox grid_zone);

#endif

