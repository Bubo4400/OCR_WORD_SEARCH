#include "contour_detection.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define IDX(x, y, w) ((y) * (w) + (x))

static void flood_fill(ImageGray *img, int x, int y, bool *visited,
                       int *minx, int *miny, int *maxx, int *maxy)
{
    int w = img->w, h = img->h;
    if (x < 0 || y < 0 || x >= w || y >= h) return;
    int idx = IDX(x, y, w);
    if (visited[idx]) return;
    if (img->bin[idx] > 128) return; // on ne garde que le noir
    visited[idx] = true;

    if (x < *minx) *minx = x;
    if (y < *miny) *miny = y;
    if (x > *maxx) *maxx = x;
    if (y > *maxy) *maxy = y;

    flood_fill(img, x + 1, y, visited, minx, miny, maxx, maxy);
    flood_fill(img, x - 1, y, visited, minx, miny, maxx, maxy);
    flood_fill(img, x, y + 1, visited, minx, miny, maxx, maxy);
    flood_fill(img, x, y - 1, visited, minx, miny, maxx, maxy);
}

int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count)
{
    if (!img || !img->bin) return -1;

    int w = img->w, h = img->h;
    bool *visited = calloc(w * h, sizeof(bool));
    BoundingBox *boxes = malloc(sizeof(BoundingBox) * 10000);
    int n = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = IDX(x, y, w);
            if (visited[idx] || img->bin[idx] > 128)
                continue;

            int minx = x, miny = y, maxx = x, maxy = y;
            flood_fill(img, x, y, visited, &minx, &miny, &maxx, &maxy);

            int bw = maxx - minx + 1;
            int bh = maxy - miny + 1;
            if (bw > 3 && bh > 3 && bw < w / 2 && bh < h / 2) {
                boxes[n++] = (BoundingBox){minx, miny, bw, bh};
                if (n >= 9999) break;
            }
        }
    }

    free(visited);
    *out_boxes = boxes;
    *count = n;
    return 0;
}

void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count)
{
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    for (int i = 0; i < count; i++) {
        SDL_Rect r = {boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h};
        SDL_RenderDrawRect(ren, &r);
    }
}

