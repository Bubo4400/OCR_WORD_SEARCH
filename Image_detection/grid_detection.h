#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "contour_detection.h"
#include "image_processing.h"

void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone);
void extract_letters_from_zones(ImageGray *img, BoundingBox word_zone, BoundingBox grid_zone);

#endif

