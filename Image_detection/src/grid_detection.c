#include "grid_detection.h"
}
}
*out_pos = pos;
return n;
}


int detect_grid_cells(const ImageGray *img, int gx, int gy,
int gw, int gh, int target_rows, int target_cols,
SDL_Rect **out_cells, int *out_n)
{
int w = img->w, h = img->h;
int rows = target_rows, cols = target_cols;
/* compute projection inside bbox */
int *vproj = calloc(gw, sizeof(int));
int *hproj = calloc(gh, sizeof(int));
if (!vproj || !hproj) return 0;
for (int y = 0; y < gh; ++y) {
for (int x = 0; x < gw; ++x) {
int v = (img->bin[(gy + y) * w + (gx + x)] == 0) ? 1 : 0;
vproj[x] += v;
hproj[y] += v;
}
}


/* If target rows/cols unknown, try to detect line peaks
which correspond to grid lines; otherwise assume evenly
spaced cells. */
int *vpeaks = NULL, *hpeaks = NULL;
int nv = find_peaks(vproj, gw, gw / 50, &vpeaks);
int nh = find_peaks(hproj, gh, gh / 50, &hpeaks);
if (cols == 0 && nv > 1) cols = nv - 1;
if (rows == 0 && nh > 1) rows = nh - 1;
if (cols <= 0) cols = 10; /* fallback */
if (rows <= 0) rows = 10;


SDL_Rect *cells = malloc(sizeof(SDL_Rect) * rows * cols);
if (!cells) return 0;
/* compute cell boundaries evenly */
for (int r = 0; r < rows; ++r) {
for (int c = 0; c < cols; ++c) {
int x0 = gx + (c * gw) / cols;
int x1 = gx + ((c + 1) * gw) / cols;
int y0 = gy + (r * gh) / rows;
int y1 = gy + ((r + 1) * gh) / rows;
cells[r * cols + c].x = x0 + 2;
cells[r * cols + c].y = y0 + 2;
cells[r * cols + c].w = (x1 - x0) - 4;
cells[r * cols + c].h = (y1 - y0) - 4;
}
}
free(vproj);
free(hproj);
free(vpeaks);
free(hpeaks);
*out_cells = cells;
*out_n = rows * cols;
return 1;
}
