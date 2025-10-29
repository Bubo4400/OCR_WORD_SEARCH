#include "grid_detection.h"
#include "image_processing.h"
#include "image_utils.h"
#include "contour_detection.h"   // <-- ajoute cette ligne
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
BoundingBox detect_main_grid(BoundingBox *boxes, int count)
{
    if (count == 0) return (BoundingBox){0,0,0,0};

    BoundingBox grid = boxes[0];
    int max_area = grid.w * grid.h;

    for (int i = 1; i < count; i++) {
        int area = boxes[i].w * boxes[i].h;
        if (area > max_area) {
            max_area = area;
            grid = boxes[i];
        }
    }

    printf("Grille détectée : x=%d y=%d w=%d h=%d\n", grid.x, grid.y, grid.w, grid.h);
    return grid;
}
void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols)
{
    int cell_w = grid.w / cols;
    int cell_h = grid.h / rows;

    // Création du dossier "out/cells"
    struct stat st = {0};
    if (stat("out", &st) == -1) mkdir("out", 0755);
    if (stat("out/cells", &st) == -1) mkdir("out/cells", 0755);

    printf("📦 Sauvegarde des cases dans out/cells/ ...\n");

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int x = grid.x + c * cell_w;
            int y = grid.y + r * cell_h;

            ImageGray *cell = crop_image(img, x, y, cell_w, cell_h);
            if (!cell) continue;

            char fname[256];
            snprintf(fname, sizeof(fname), "out/cells/cell_%02d_%02d.bmp", r, c);
            save_gray_bmp(cell, fname);
            free_image_gray(cell);
        }
    }
    printf("✅ %d x %d cases enregistrées.\n", rows, cols);
}

void extract_side_words(ImageGray *img, BoundingBox grid)
{
    printf("Recherche des mots à côté de la grille...\n");
    int margin = 10;

    // Zone gauche
    BoundingBox left = {0, 0, grid.x - margin, img->h};
    // Zone droite
    BoundingBox right = {grid.x + grid.w + margin, 0, img->w - (grid.x + grid.w + margin), img->h};

    printf("Zone gauche : x=%d w=%d\n", left.x, left.w);
    printf("Zone droite : x=%d w=%d\n", right.x, right.w);

    // Tu peux ici binariser et extraire les mots à partir de ces zones.
}

