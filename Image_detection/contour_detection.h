#ifndef CONTOUR_DETECTION_H
#define CONTOUR_DETECTION_H

#include "image_processing.h"

typedef struct {
    int x, y, w, h;
} BoundingBox;

int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count);
void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count);

#endif

