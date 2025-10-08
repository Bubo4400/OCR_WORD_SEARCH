#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H


#include "image_processing.h"
#include <SDL2/SDL.h>


/* Detect grid bounding box [x,y,w,h]. Returns 1 on success. */
int detect_grid_bbox(const ImageGray *img, int *rx, int *ry,
int *rw, int *rh);


/* Given bbox and desired rows/cols, compute cell boxes by
projecting horizontal and vertical peaks.
If rows/cols == 0, the function tries to infer them. */
int detect_grid_cells(const ImageGray *img, int gx, int gy,
int gw, int gh, int target_rows, int target_cols,
SDL_Rect **out_cells, int *out_n);


#endif
