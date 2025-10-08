#ifndef CONTOUR_DETECTION_H
#define CONTOUR_DETECTION_H


#include "image_processing.h"
#include <SDL2/SDL.h>


/* Find connected components in a rectangular region of bin image.
Returns number of components and fills bbox list (caller frees). */
int find_components(const ImageGray *img, int x, int y, int w, int h,
SDL_Rect **out_bboxes, int *out_n);


#endif
