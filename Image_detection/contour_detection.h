#ifndef CONTOUR_DETECTION_H
#define CONTOUR_DETECTION_H

#include "image_processing.h"
/* Bounding box structure for detected connected components */
typedef struct {
    int x, y, w, h;
} BoundingBox;
/* Find connected components (contours) in a binarized grayscale image.
  - img:input binary-like image (0..255), foreground expected near 0
  - out_boxes: pointer that will be set to an allocated array of BoundingBox
 - count: pointer to integer that will receive the number of boxes
  Returns 0 on success, <0 on failure.

  The caller is responsible for freeing *out_boxes be carful
 */
int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count);
/* Draw rectangles for the supplied boxes using the provided renderer */
void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count);

#endif

