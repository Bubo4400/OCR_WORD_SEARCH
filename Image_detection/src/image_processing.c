#include "image_processing.h"
{
if (!img) return;
free(img->gray);
free(img->bin);
free(img);
}


int otsu_binarize(ImageGray *img)
{
int w = img->w, h = img->h;
unsigned long hist[256] = {0};
for (int i = 0; i < w * h; ++i)
hist[img->gray[i]]++;
unsigned long total = w * h;
double sum = 0;
for (int t = 0; t < 256; ++t)
sum += t * hist[t];
double sumB = 0;
unsigned long wB = 0;
double varMax = 0;
int threshold = 128;
for (int t = 0; t < 256; ++t) {
wB += hist[t];
if (wB == 0) continue;
double wF = total - wB;
if (wF == 0) break;
sumB += (double)(t * hist[t]);
double mB = sumB / wB;
double mF = (sum - sumB) / wF;
double varBetween = (double)wB * wF * (mB - mF) * (mB - mF);
if (varBetween > varMax) {
varMax = varBetween;
threshold = t;
}
}
for (int i = 0; i < w * h; ++i)
img->bin[i] = (img->gray[i] > threshold) ? 255 : 0;
return threshold;
}


void save_surface_png(SDL_Surface *surf, const char *path)
{
IMG_SavePNG(surf, path);
}
