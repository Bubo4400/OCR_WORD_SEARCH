#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "contour_detection.h"
#include "image_processing.h"

BoundingBox detect_main_grid(BoundingBox *boxes, int count);
void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols);
void extract_side_words(ImageGray *img, BoundingBox grid);

/* ✅ Ajouts */
int detect_peaks_from_centers(BoundingBox *boxes, int count, int axis,
                              int *peaks, int *peak_count, int approx_cells);
int detect_grid_from_boxes(BoundingBox *boxes, int count,
                           int *rows, int *nrows, int *cols, int *ncols);

#endif

