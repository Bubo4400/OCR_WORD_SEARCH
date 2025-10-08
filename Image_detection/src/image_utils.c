#include "image_utils.h"
#include <stdlib.h>

SDL_Surface *load_image_grayscale(const char *path)
{
    SDL_Surface *img = IMG_Load(path);
    if (!img) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return NULL;
    }

    SDL_Surface *gray = SDL_CreateRGBSurfaceWithFormat(0, img->w, img->h, 32,
        SDL_PIXELFORMAT_RGBA32);
    SDL_LockSurface(img);
    SDL_LockSurface(gray);

    Uint32 *src = img->pixels;
    Uint32 *dst = gray->pixels;
    for (int y = 0; y < img->h; ++y) {
        for (int x = 0; x < img->w; ++x) {
            Uint8 r, g, b;
            SDL_GetRGB(src[y * img->w + x], img->format, &r, &g, &b);
            Uint8 grayv = 0.299 * r + 0.587 * g + 0.114 * b;
            dst[y * img->w + x] = SDL_MapRGBA(gray->format, grayv, grayv, grayv, 255);
        }
    }

    SDL_UnlockSurface(gray);
    SDL_UnlockSurface(img);
    SDL_FreeSurface(img);
    return gray;
}

void threshold_image(SDL_Surface *surf, Uint8 thresh)
{
    SDL_LockSurface(surf);
    Uint32 *p = surf->pixels;
    for (int i = 0; i < surf->w * surf->h; ++i) {
        Uint8 r, g, b;
        SDL_GetRGB(p[i], surf->format, &r, &g, &b);
        Uint8 v = r > thresh ? 255 : 0;
        p[i] = SDL_MapRGBA(surf->format, v, v, v, 255);
    }
    SDL_UnlockSurface(surf);
}

void draw_rect(SDL_Renderer *ren, Rect r, SDL_Color color)
{
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {r.x, r.y, r.w, r.h};
    SDL_RenderDrawRect(ren, &rect);
}

