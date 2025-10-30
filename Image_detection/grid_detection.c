#include "grid_detection.h"
#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

/* ✅ Détection des lignes/colonnes à partir des lettres */
int detect_peaks_from_centers(BoundingBox *boxes, int count, int axis,
                              int *peaks, int *peak_count, int approx_cells)
{
    if(count<=0){ *peak_count=0; return -1; }
    int N = count;
    int *centers = malloc(sizeof(int)*N);
    for(int i=0;i<N;i++){
        centers[i] = (axis==0)
            ? boxes[i].y + boxes[i].h/2
            : boxes[i].x + boxes[i].w/2;
    }
    // tri simple
    for(int i=0;i<N-1;i++)
        for(int j=i+1;j<N;j++)
            if(centers[i]>centers[j]){int t=centers[i]; centers[i]=centers[j]; centers[j]=t;}

    int k=0,i=0;
    while(i<N){
        int sum=centers[i], cnt=1;
        int j=i+1;
        while(j<N && centers[j]-centers[j-1]<12){
            sum+=centers[j]; cnt++; j++;
        }
        peaks[k++] = sum/cnt;
        i=j;
    }
    *peak_count = k;
    free(centers);
    return 0;
}

int detect_grid_from_boxes(BoundingBox *boxes, int count,
                           int *rows, int *nrows, int *cols, int *ncols)
{
    detect_peaks_from_centers(boxes, count, 0, rows, nrows, 0);
    detect_peaks_from_centers(boxes, count, 1, cols, ncols, 0);
    printf("🧮 Grille détectée : %d lignes × %d colonnes\n", *nrows, *ncols);
    return 0;
}

/* Fonctions existantes */
BoundingBox detect_main_grid(BoundingBox *boxes, int count){
    if(count==0) return (BoundingBox){0,0,0,0};
    BoundingBox grid = boxes[0];
    int max_area = grid.w*grid.h;
    for(int i=1;i<count;i++){
        int area = boxes[i].w*boxes[i].h;
        if(area>max_area){
            max_area = area;
            grid = boxes[i];
        }
    }
    printf("Grille détectée : x=%d y=%d w=%d h=%d\n",grid.x,grid.y,grid.w,grid.h);
    return grid;
}

void extract_cells(ImageGray *img, BoundingBox grid, int rows, int cols){
    int cell_w = grid.w/cols;
    int cell_h = grid.h/rows;

    struct stat st={0};
    if(stat("out",&st)==-1) mkdir("out",0755);
    if(stat("out/cells",&st)==-1) mkdir("out/cells",0755);

    printf("📦 Sauvegarde des cases dans out/cells/ ...\n");
    for(int r=0;r<rows;r++){
        for(int c=0;c<cols;c++){
            int x = grid.x + c*cell_w;
            int y = grid.y + r*cell_h;
            ImageGray *cell = crop_image(img,x,y,cell_w,cell_h);
            if(!cell) continue;
            char fname[256];
            snprintf(fname,sizeof(fname),"out/cells/cell_%02d_%02d.bmp",r,c);
            save_gray_bmp(cell,fname);
            free_image_gray(cell);
        }
    }
    printf("✅ %d x %d cases enregistrées.\n",rows,cols);
}

void extract_side_words(ImageGray *img, BoundingBox grid){
    printf("Recherche des mots à côté de la grille...\n");
    int margin = 10;
    BoundingBox left = {0,0,grid.x-margin,img->h};
    BoundingBox right = {grid.x+grid.w+margin,0,img->w-(grid.x+grid.w+margin),img->h};
    printf("Zone gauche : x=%d w=%d\n",left.x,left.w);
    printf("Zone droite : x=%d w=%d\n",right.x,right.w);
}

