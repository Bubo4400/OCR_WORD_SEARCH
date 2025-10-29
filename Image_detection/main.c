#include "image_processing.h"
#include "contour_detection.h"
#include "grid_detection.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        SDL_Log("Usage: %s <image.png>", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    ImageGray *img = load_image_gray(argv[1]);
    if (!img) return 1;
    preprocess_image(img);

    BoundingBox *boxes = NULL;
    int count = 0;
    find_contours(img, &boxes, &count);

    BoundingBox grid = detect_main_grid(boxes, count);
    extract_cells(img, grid, 10, 10);
    extract_side_words(img, grid);

    // Affichage SDL
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, img->w, img->h, 32, SDL_PIXELFORMAT_ARGB8888);
    Uint32 *pixels = (Uint32*)surf->pixels;
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            Uint8 val = img->bin[y * img->w + x];
            pixels[y * img->w + x] = SDL_MapRGB(surf->format, val, val, val);
        }
    }

    SDL_Window *win = SDL_CreateWindow("OCR - Grille détectée",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, img->w, img->h, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) running = false;

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        draw_contours(ren, boxes, count);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_FreeSurface(surf);
    free(boxes);
    free_image_gray(img);

    IMG_Quit();
    SDL_Quit();
    return 0;
}

