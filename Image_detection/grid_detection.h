#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include "contour_detection.h"
#include "image_processing.h"

int detect_peaks_from_centers(BoundingBox *boxes, int count, int axis,
                              int *peaks, int *peak_count, int approx_cells);
int detect_grid_from_boxes(BoundingBox *boxes, int count,
                           int *rows, int *nrows, int *cols, int *ncols);

/* ✅ Nouvelle version à 4 arguments */
BoundingBox detect_main_grid(BoundingBox *boxes, int count, int img_w, int img_h);

void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols);
void extract_side_words(ImageGray *img, BoundingBox grid);

/* ✅ Prototype manquant ajouté ici */
BoundingBox detect_word_zone(ImageGray *img, BoundingBox grid);
void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone);
#endif

