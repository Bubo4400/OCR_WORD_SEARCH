#include "grid_detection.h"
#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone) {
    int w = img->w, h = img->h;

    int *col_sum = calloc(w, sizeof(int));
    int *row_sum = calloc(h, sizeof(int));

    //  Profil vertical (texte à gauche ou droite)
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            if (img->pixels[y * w + x] < 128)
                col_sum[x]++;

    //  Profil horizontal (texte en haut ou bas)
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (img->pixels[y * w + x] < 128)
                row_sum[y]++;

    // Lissage simple
    int window = 10;
    for (int x = window; x < w - window; x++) {
        int sum = 0;
        for (int k = -window; k <= window; k++)
            sum += col_sum[x + k];
        col_sum[x] = sum / (2 * window + 1);
    }
    for (int y = window; y < h - window; y++) {
        int sum = 0;
        for (int k = -window; k <= window; k++)
            sum += row_sum[y + k];
        row_sum[y] = sum / (2 * window + 1);
    }

    // Recherche de la plus grande "zone vide"
    int max_col = 0, max_row = 0;
    for (int x = 0; x < w; x++) if (col_sum[x] > max_col) max_col = col_sum[x];
    for (int y = 0; y < h; y++) if (row_sum[y] > max_row) max_row = row_sum[y];

    int thr_col = max_col / 5;
    int thr_row = max_row / 5;

    int best_col_gap = 0, best_row_gap = 0;
    int best_col_len = 0, best_row_len = 0;
    int in_gap = 0, start = 0;

    // Colonnes (vertical)
    for (int x = 0; x < w; x++) {
        if (col_sum[x] < thr_col) {
            if (!in_gap) { in_gap = 1; start = x; }
        } else if (in_gap) {
            int len = x - start;
            if (len > best_col_len) { best_col_len = len; best_col_gap = x; }
            in_gap = 0;
        }
    }

    in_gap = 0;
    for (int y = 0; y < h; y++) {
        if (row_sum[y] < thr_row) {
            if (!in_gap) { in_gap = 1; start = y; }
        } else if (in_gap) {
            int len = y - start;
            if (len > best_row_len) { best_row_len = len; best_row_gap = y; }
            in_gap = 0;
        }
    }

    if (best_col_len >= best_row_len) {
        int sep = best_col_gap;
        if (sep < w / 2) {
            word_zone->x = 0; word_zone->y = 0; word_zone->w = sep; word_zone->h = h;
            grid_zone->x = sep; grid_zone->y = 0; grid_zone->w = w - sep; grid_zone->h = h;
        } else {
            grid_zone->x = 0; grid_zone->y = 0; grid_zone->w = sep; grid_zone->h = h;
            word_zone->x = sep; word_zone->y = 0; word_zone->w = w - sep; word_zone->h = h;
        }
    } else {
        int sep = best_row_gap;
        if (sep < h / 2) {
            word_zone->x = 0; word_zone->y = 0; word_zone->w = w; word_zone->h = sep;
            grid_zone->x = 0; grid_zone->y = sep; grid_zone->w = w; grid_zone->h = h - sep;
        } else {
            grid_zone->x = 0; grid_zone->y = 0; grid_zone->w = w; grid_zone->h = sep;
            word_zone->x = 0; word_zone->y = sep; word_zone->w = w; word_zone->h = h - sep;
        }
    }

    free(col_sum);
    free(row_sum);
}

int detect_peaks_from_centers(BoundingBox *boxes, int count, int axis,
                              int *peaks, int *peak_count, int approx_cells)
{
    if (count <= 0) { *peak_count = 0; return -1; }
    int N = count;
    int *centers = malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) {
        centers[i] = (axis == 0)
            ? boxes[i].y + boxes[i].h / 2
            : boxes[i].x + boxes[i].w / 2;
    }

    for (int i = 0; i < N - 1; i++)
        for (int j = i + 1; j < N; j++)
            if (centers[i] > centers[j]) {
                int t = centers[i];
                centers[i] = centers[j];
                centers[j] = t;
            }

    int k = 0, i = 0;
    while (i < N) {
        int sum = centers[i], cnt = 1;
        int j = i + 1;
        while (j < N && centers[j] - centers[j - 1] < 12) {
            sum += centers[j];
            cnt++;
            j++;
        }
        peaks[k++] = sum / cnt;
        i = j;
    }
    *peak_count = k;
    free(centers);
    return 0;
}

int detect_grid_from_boxes(BoundingBox *boxes, int count,
                           int *rows, int *nrows, int *cols, int *ncols)
{
    detect_peaks_from_centers(boxes, count, 0, rows, nrows, 0);
    detect_peaks_from_centers(boxes, count, 1, cols, ncols, 0);
    return 0;
}

BoundingBox detect_main_grid(BoundingBox *boxes, int count, int img_w, int img_h) {
    if (count == 0) return (BoundingBox){0,0,0,0};

    int cx = img_w / 2;
    int cy = img_h / 2;
    BoundingBox best = boxes[0];
    double best_score = -1.0;

    for (int i = 0; i < count; i++) {
        BoundingBox b = boxes[i];
        double area = b.w * b.h;

        // distance au centre
        double dx = (b.x + b.w / 2) - cx;
        double dy = (b.y + b.h / 2) - cy;
        double dist = sqrt(dx * dx + dy * dy);

        // Score = aire / (distance + 1)
        double score = area / (dist + 1.0);

        if (score > best_score) {
            best_score = score;
            best = b;
        }
    }

    printf("Grille principale détectée : x=%d y=%d w=%d h=%d\n", best.x, best.y, best.w, best.h);
    return best;
}

BoundingBox detect_word_zone(ImageGray *img, BoundingBox grid) {
    int margin = 10;
    BoundingBox zone = {0, 0, 0, 0};
    // On estime que la liste de mots est à droite ou en dessous
    if (img->w - (grid.x + grid.w) > grid.x) {
        // zone à droite
        zone.x = grid.x + grid.w + margin;
        zone.y = grid.y;
        zone.w = img->w - zone.x - margin;
        zone.h = grid.h;
    } else {
        // zone en dessous
        zone.x = grid.x;
        zone.y = grid.y + grid.h + margin;
        zone.w = grid.w;
        zone.h = img->h - zone.y - margin;
    }
    return zone;
}

void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols) {
    int cell_w = grid.w / cols;
    int cell_h = grid.h / rows;

    struct stat st = {0};
    if (stat("out", &st) == -1) mkdir("out", 0755);
    if (stat("out/cells", &st) == -1) mkdir("out/cells", 0755);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int x = grid.x + c * cell_w;
            int y = grid.y + r * cell_h;
            ImageGray *cell = crop_image(img, x, y, cell_w, cell_h);
            if (!cell) continue;
            char fname[256];
            snprintf(fname, sizeof(fname), "out/cells/cell_%02d_%02d.bmp", r, c);
            save_gray_bmp(cell, fname);
            free_image_gray(cell);
        }
    }
}

