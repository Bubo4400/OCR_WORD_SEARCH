#include "contour_detection.h"
#include <stdlib.h>
#include <string.h>


/* 4-connected BFS */
int find_components(const ImageGray *img, int ox, int oy, int ow, int oh,
SDL_Rect **out_bboxes, int *out_n)
{
int W = img->w;
unsigned char *visited = calloc(ow * oh, 1);
if (!visited) return 0;
SDL_Rect *bboxes = malloc(sizeof(SDL_Rect) * 256);
int nb = 0;
for (int yy = 0; yy < oh; ++yy) {
for (int xx = 0; xx < ow; ++xx) {
int idx = yy * ow + xx;
if (visited[idx]) continue;
int gx = ox + xx;
int gy = oy + yy;
if (img->bin[gy * W + gx] != 0) continue; /* background is 255 */
/* start BFS */
int qcap = 1024;
int qh = 0;
int qt = 0;
int *qx = malloc(sizeof(int) * qcap);
int *qy = malloc(sizeof(int) * qcap);
if (!qx || !qy) { free(qx); free(qy); free(visited); return 0; }
/* enqueue */
qx[qt] = xx; qy[qt] = yy; qt++;
visited[idx] = 1;
int minx = xx, maxx = xx, miny = yy, maxy = yy;
while (qh < qt) {
int cx = qx[qh];
int cy = qy[qh];
qh++;
/* neighbors */
const int dx[4] = {1,-1,0,0};
const int dy[4] = {0,0,1,-1};
for (int k = 0; k < 4; ++k) {
int nx = cx + dx[k];
int ny = cy + dy[k];
if (nx < 0 || nx >= ow || ny < 0 || ny >= oh) continue;
int nidx = ny * ow + nx;
if (visited[nidx]) continue;
int gx2 = ox + nx;
int gy2 = oy + ny;
if (img->bin[gy2 * W + gx2] != 0) continue;
/* push */
if (qt >= qcap) {
qcap *= 2;
qx = realloc(qx, sizeof(int) * qcap);
qy = realloc(qy, sizeof(int) * qcap);
}
}
