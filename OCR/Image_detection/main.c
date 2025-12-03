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
        // On vérifie qu'on a au moins 2 arguments : 
        // 1 : le chemin de l'image d'entrée
        // 2  :le nom pour les fichiers de sortie
        return 1; // Si pas assez d'arguments => on quitte 
    }

    const char *input_path = argv[1];   // Chemin de l'image à charger
    const char *output_prefix = argv[2]; // nom utilisé pour les fichiers de sortie
    char output_file[256];              // Tableau pour stocker le nom d'un fichier de sortie

    // Init de la bibliothèque SDL 
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1; // Si l'initialisation échoue, on quitte
    }

    // Initialisation du module pour charger les images PNG et JPG
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    // Chargement de l'image originale en niveau de gris
    ImageGray *original = load_image_gray(input_path);
    if (!original) {
        return 1; // S il  échoue, on quitte
    }

    // On charge une copie de l'image pour le traitement
    ImageGray *img = load_image_gray(input_path);

    // Prétraitement : contraste + binarisation
    preprocess_image(img);

    // Sauvegarde de l'image prétraitée (nom à définir plus tard)
    save_gray_bmp(img, output_file);

    // Création d'une fenêtre SDL pour afficher l'image de fin
    SDL_Window *window = SDL_CreateWindow("OCR Preview",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,img->w, img->h, 0);

    // Création d'un moteur de rendu (pour dessiner)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Création d'une texture (zone graphique où l'image sera affichée)
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, img->w, img->h);

    // Détection des contours (lettres, zones sombres)
    BoundingBox *boxes = NULL;
    int count = 0;
    find_contours(img, &boxes, &count);

    // Détection automatique des zones :mots & grille
    BoundingBox word_zone, grid_zone;
    detect_zones_auto(img, &word_zone, &grid_zone);

    // Extraction des lettres trouvées dans les zones
    extract_letters_from_zones(img, word_zone, grid_zone);

    // Création d’un tableau de pixels (RGB) pour l’affichage SDL
    Uint8 *pixels = malloc(img->w * img->h * 3);
    for (int i = 0; i < img->w * img->h; i++) {
        // On duplique la valeur de gris dans R, G et B pour obtenir une image "grise"
	// im en niveau de gris = 1 octet et en RGB 3 octets car 3 couleurs
        pixels[i*3 + 0] = img->pixels[i];// valeur de i entre 0 et 255 et copier dans les 3 rouges verts et bleus
        pixels[i*3 + 1] = img->pixels[i];
        pixels[i*3 + 2] = img->pixels[i];
    }

    // On met à jour la texture avec les pixels de l’image
    SDL_UpdateTexture(texture, NULL, pixels, img->w * 3);

    // Variables pour gérer la rotation et la boucle d'affichage
    double angle = 0.0; // Angle actuel de rotation
    int running = 1;    // Booléen pour savoir si le programme tourne
    SDL_Event e;        // Variable pour gérer les événements clavier / souris

    //Boucle principale du programme 
    while (running) {
        // On récupère les événements SDL (touche, clic, etc.)
        while (SDL_PollEvent(&e)) {
            // Si on ferme la fenêtre ou appuie sur Echap -> on quitte
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                running = 0;

            // Touche "r" = rotation à droite(+5 degrés)
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
                angle += 5.0;
            //Touche "l"=rotation à gauche (-5 degrés)
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_l)
                angle -= 5.0;

            // Si on a appuyé sur "r" ou "l" -> il faut mettre  à jour l'image
            if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_r || e.key.keysym.sym == SDLK_l)) {
                // On crée une nouvelle image tournée à l'angle choisi
                ImageGray *rot = rotate_image_centered(original, angle);
                if (rot) {
                    //On remplace l'ancienne image par la nouvelle
                    free_image_gray(img);
                    img = rot;

                    //On refait le prétraitement et la détection
                    preprocess_image(img);
                    save_gray_bmp(img, output_file);
                    free(boxes);
                    find_contours(img, &boxes, &count);

                    detect_zones_auto(img, &word_zone, &grid_zone);
                    extract_letters_from_zones(img, word_zone, grid_zone);

                    // On reconstruit les pixels pour l’affichage
                    free(pixels);
                    pixels = malloc(img->w * img->h * 3);
                    for (int i = 0; i < img->w * img->h; i++) {
                        pixels[i*3 + 0] = img->pixels[i];
                        pixels[i*3 + 1] = img->pixels[i];
                        pixels[i*3 + 2] = img->pixels[i];
                    }

                    //On détruit et recrée la texture(car taille/rotation changée)
                    SDL_DestroyTexture(texture);
                    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING, img->w, img->h);
                    SDL_UpdateTexture(texture, NULL, pixels, img->w * 3);
                    SDL_SetWindowSize(window, img->w, img->h);
                }
            }
        }

        // Affichage de l'image et rectangles
        SDL_RenderClear(renderer);            // Efface la fenêtre
        SDL_RenderCopy(renderer, texture, NULL, NULL); // Dessine l'image sur l'écran

        // Couleur rouge pour les contours détectés
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < count; i++) {
            SDL_Rect r = { boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h };
            SDL_RenderDrawRect(renderer, &r); // Dessine chaque contour
        }

        // Couleur verte pour les zones "mots" et "grille"
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect wr = { word_zone.x, word_zone.y, word_zone.w, word_zone.h };
        SDL_Rect gr = { grid_zone.x, grid_zone.y, grid_zone.w, grid_zone.h };
        SDL_RenderDrawRect(renderer, &wr);
        SDL_RenderDrawRect(renderer, &gr);

        SDL_RenderPresent(renderer); // Met à jour affichage
        SDL_Delay(16); // Petite pause (~60 FPS)
    }

    // Nettoyage de la mémoire  anti memory leak
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

