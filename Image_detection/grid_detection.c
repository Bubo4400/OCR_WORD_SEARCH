#include "grid_detection.h"
#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

static int is_valid_letter(BoundingBox b, int img_w, int img_h) {
    if (b.w < 5 || b.h < 5) return 0;
    if (b.w > img_w / 5 || b.h > img_h / 5) return 0;
    if ((float)b.w / b.h > 3.0 || (float)b.h / b.w > 3.0) return 0;
    return 1;
}

void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone) {
    int w = img->w, h = img->h;
    int *col_density = calloc(w, sizeof(int));

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (img->pixels[y*w + x] < 128)
                col_density[x]++;

    int threshold = h / 8;
    int best_gap = 0, best_start = 0;
    int gap = 0, gap_start = 0;

    for (int x = 0; x < w; x++) {
        if (col_density[x] < threshold) {
            if (gap == 0) gap_start = x;
            gap++;
        } else {
            if (gap > best_gap) {
                best_gap = gap;
                best_start = gap_start;
            }
            gap = 0;
        }
    }

    int sep = best_start + best_gap / 2;

    long left_density = 0, right_density = 0;
    for (int x = 0; x < sep; x++) left_density += col_density[x];
    for (int x = sep; x < w; x++) right_density += col_density[x];

    if (left_density > right_density) {
        *word_zone = (BoundingBox){0, 0, sep, h};
        *grid_zone = (BoundingBox){sep, 0, w - sep, h};
    } else {
        *word_zone = (BoundingBox){sep, 0, w - sep, h};
        *grid_zone = (BoundingBox){0, 0, sep, h};
    }

    free(col_density);
}

void extract_letters_from_zones(ImageGray *img, BoundingBox word_zone, BoundingBox grid_zone) {
    struct stat st = {0};
    if (stat("out", &st) == -1) mkdir("out", 0755);
    if (stat("out/grid", &st) == -1) mkdir("out/grid", 0755);
    if (stat("out/words", &st) == -1) mkdir("out/words", 0755);

    BoundingBox *boxes = NULL;
    int count = 0;
    find_contours(img, &boxes, &count);

    for (int i = 0; i < count; i++) {
        BoundingBox b = boxes[i];
        if (!is_valid_letter(b, img->w, img->h)) continue;

        if (b.x >= grid_zone.x && b.x + b.w <= grid_zone.x + grid_zone.w) {
            ImageGray *cell = crop_image(img, b.x, b.y, b.w, b.h);
            if (cell) {
                char fname[256];
                save_gray_bmp(cell, fname);
                free_image_gray(cell);
            }
        }

        if (b.x >= word_zone.x && b.x + b.w <= word_zone.x + word_zone.w) {
            ImageGray *cell = crop_image(img, b.x, b.y, b.w, b.h);
            if (cell) {
                char fname[256];
                save_gray_bmp(cell, fname);
                free_image_gray(cell);
            }
        }
    }
    free(boxes);
}

