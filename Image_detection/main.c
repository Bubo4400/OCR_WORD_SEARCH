#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_processing.h"
#include "image_utils.h"
#include "contour_detection.h"
#include "grid_detection.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }
    const char *input_path = argv[1];
    const char *output_prefix = argv[2];
    char output_file[256];
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    ImageGray *original = load_image_gray(input_path);
    if (!original) {
        return 1;
    }

    ImageGray *img = load_image_gray(input_path);
    preprocess_image(img);
    save_gray_bmp(img, output_file);

    SDL_Window *window = SDL_CreateWindow("OCR Preview",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        img->w, img->h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING, img->w, img->h);

    BoundingBox *boxes = NULL;
    int count = 0;
    find_contours(img, &boxes, &count);
    BoundingBox word_zone, grid_zone;
    detect_zones_auto(img, &word_zone, &grid_zone);
    extract_letters_from_zones(img, word_zone, grid_zone);

    Uint8 *pixels = malloc(img->w * img->h * 3);
    for (int i = 0; i < img->w * img->h; i++) {
        pixels[i*3 + 0] = img->pixels[i];
        pixels[i*3 + 1] = img->pixels[i];
        pixels[i*3 + 2] = img->pixels[i];
    }
    SDL_UpdateTexture(texture, NULL, pixels, img->w * 3);

    double angle = 0.0;
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                running = 0;

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
                angle += 5.0;

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_l)
                angle -= 5.0;

            if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_r || e.key.keysym.sym == SDLK_l)) {
                ImageGray *rot = rotate_image_centered(original, angle);
                if (rot) {
                    free_image_gray(img);
                    img = rot;
                    preprocess_image(img);
                    save_gray_bmp(img, output_file);

                    free(boxes);
                    find_contours(img, &boxes, &count);

                    detect_zones_auto(img, &word_zone, &grid_zone);
                    extract_letters_from_zones(img, word_zone, grid_zone);

                    free(pixels);
                    pixels = malloc(img->w * img->h * 3);
                    for (int i = 0; i < img->w * img->h; i++) {
                        pixels[i*3 + 0] = img->pixels[i];
                        pixels[i*3 + 1] = img->pixels[i];
                        pixels[i*3 + 2] = img->pixels[i];
                    }
                    SDL_DestroyTexture(texture);
                    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING, img->w, img->h);
                    SDL_UpdateTexture(texture, NULL, pixels, img->w * 3);
                    SDL_SetWindowSize(window, img->w, img->h);
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < count; i++) {
            SDL_Rect r = { boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h };
            SDL_RenderDrawRect(renderer, &r);
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect wr = { word_zone.x, word_zone.y, word_zone.w, word_zone.h };
        SDL_Rect gr = { grid_zone.x, grid_zone.y, grid_zone.w, grid_zone.h };
        SDL_RenderDrawRect(renderer, &wr);
        SDL_RenderDrawRect(renderer, &gr);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    free(boxes);
    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    free_image_gray(img);
    free_image_gray(original);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

