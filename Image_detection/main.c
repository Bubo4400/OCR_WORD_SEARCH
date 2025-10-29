#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_processing.h"
#include "image_utils.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Utilisation : %s <image_entree> <nom_sortie>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_prefix = argv[2];
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s.bmp", output_prefix);

    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL : %s\n", SDL_GetError());
        return 1;
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    // Chargement et prétraitement
    ImageGray *img = load_image_gray(input_path);
    if (!img) {
        fprintf(stderr, "Erreur : impossible de charger %s\n", input_path);
        return 1;
    }

    printf("Prétraitement de l'image...\n");
    preprocess_image(img);
    save_gray_bmp(img, output_file);
    printf("Image sauvegardée sous : %s\n", output_file);

    // Création d’une surface SDL pour affichage
    SDL_Window *window = SDL_CreateWindow("OCR Preview",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          img->w, img->h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             img->w, img->h);

    // Conversion ImageGray → texture SDL
    Uint8 *pixels = malloc(img->w * img->h * 3);
    for (int i = 0; i < img->w * img->h; i++) {
        pixels[i * 3 + 0] = img->pixels[i];
        pixels[i * 3 + 1] = img->pixels[i];
        pixels[i * 3 + 2] = img->pixels[i];
    }
    SDL_UpdateTexture(texture, NULL, pixels, img->w * 3);

    // Boucle d’affichage
    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // Libération mémoire
    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    free_image_gray(img);

    IMG_Quit();
    SDL_Quit();
    return 0;
}

