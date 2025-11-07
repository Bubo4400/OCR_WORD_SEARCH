#include "grid_detection.h"
#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

// Vérifie si une bounding box ressemble à une lettre (tailles et ratio a peu pres normal)
static int is_valid_letter(BoundingBox b, int img_w, int img_h) {
    if (b.w < 5 || b.h < 5) return 0; // trop petit -> probablement du bruit
    if (b.w > img_w / 5 || b.h > img_h / 5) return 0; // trop grand -> pas une lettre
    if ((float)b.w / b.h > 3.0 || (float)b.h / b.w > 3.0) return 0; // ratio bizarre -> rejette
    return 1;
}

// détection automatique des zones "mot" et "grille" => densité verticale
void detect_zones_auto(ImageGray *img, BoundingBox *word_zone, BoundingBox *grid_zone) {
    int w = img->w, h = img->h;
    int *col_density = calloc(w, sizeof(int)); // densité verticale : nombre de pixels noirs par colonne

    // Calcule la densité verticale : pour chaque pixel noir,on incrémente la colonne correspondante
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (img->pixels[y*w + x] < 128)
                col_density[x]++;
    // Seuil pour considérer une colonne "peu dense" (ce qu'on veut : séparation)
    int threshold = h / 8;
    int best_gap = 0, best_start = 0;
    int gap = 0, gap_start = 0;

    // Cherche la plus grande séquence (gap) de colonnes peu denses: probablement séparation entre zones
    for (int x = 0; x < w; x++) {
        if (col_density[x] < threshold) {
            if (gap == 0) gap_start = x;
            gap++;
        } else {
            if (gap > best_gap) {
                best_gap = gap;
                best_start = gap_start;
            }
            gap = 0;
        }
    }

    // Séparateur estimé (colonne médiane du plus grand gap)
    int sep = best_start + best_gap / 2;

    // Somme des densités à gauche et à droite pour déterminer quelle partie est "mots" (plus dense)
    long left_density = 0, right_density = 0;
    for (int x = 0; x < sep; x++) left_density += col_density[x];
    for (int x = sep; x < w; x++) right_density += col_density[x];

    // On suppose que la partie la plus dense contient les mots, l'autre la grille
    if (left_density > right_density) {
        *word_zone = (BoundingBox){0, 0, sep, h};
        *grid_zone = (BoundingBox){sep, 0, w - sep, h};
    } else {
        *word_zone = (BoundingBox){sep, 0, w - sep, h};
        *grid_zone = (BoundingBox){0, 0, sep, h};
    }

    free(col_density);
}

// Extrait les lettres trouvées dans les zones et sauvegarde chaque lettre dans le dossier out/
void extract_letters_from_zones(ImageGray *img, BoundingBox word_zone, BoundingBox grid_zone) {
    struct stat st = {0};
    // Crée les dossiers 'out', 'out/grid' et 'out/words' s'ils n'existent pas
    if (stat("out", &st) == -1) mkdir("out", 0755);
    if (stat("out/grid", &st) == -1) mkdir("out/grid", 0755);
    if (stat("out/words", &st) == -1) mkdir("out/words", 0755);

    //Trouve tous les contours (bounding boxes) dans l'image complète
    BoundingBox *boxes = NULL;
    int count = 0;
    find_contours(img, &boxes, &count);

    // Parcours toutes les boîtes et garde celles qui ressemblent à des lettres
    for (int i = 0; i < count; i++) {
        BoundingBox b = boxes[i];
        if (!is_valid_letter(b, img->w, img->h)) continue; // filtre

        // Si la boîte est dans la zone 'grid', on crop et sauvegarde
        if (b.x >= grid_zone.x && b.x + b.w <= grid_zone.x + grid_zone.w) {
            ImageGray *cell = crop_image(img, b.x, b.y, b.w, b.h);
            if (cell) {
                char fname[256];
                //  fname n'est pas rempli jsp si il faut que je le fasse
                save_gray_bmp(cell, fname); // sauvegarde du motif découpé
                free_image_gray(cell);
            }
        }

        // Si la boîte est dans la zone 'words', idem : crop et sauvegarde
        if (b.x >= word_zone.x && b.x + b.w <= word_zone.x + word_zone.w) {
            ImageGray *cell = crop_image(img, b.x, b.y, b.w, b.h);
            if (cell) {
                char fname[256];
                // fname non initialisé -> sauvegarde vers un nom indéfini
                save_gray_bmp(cell, fname);
                free_image_gray(cell);
            }
        }
    }
    free(boxes); // libère la mémoire des boxes allouées:find_contours sinon memory leaks
}

