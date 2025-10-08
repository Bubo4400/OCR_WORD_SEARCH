#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "image_utils.h"
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        SDL_Log("Usage: %s <image.png>", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Surface *img = load_image_grayscale(argv[1]);
    if (!img) return 1;
    threshold_image(img, 180);

    SDL_Window *win = SDL_CreateWindow("WordSearch OCR - Grid Detection",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        img->w, img->h, 0);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, img);

    // --- Dessin des contours simplifiés (exemple statique pour test)
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color red = {255, 0, 0, 255};

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);

        // Exemple : dessine la zone de grille et les zones lettres (fictif ici)
        draw_rect(ren, (Rect){50, 50, img->w - 100, img->h - 100}, green);
        draw_rect(ren, (Rect){20, 50, 150, img->h - 100}, red);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

