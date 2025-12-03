#include "contour_detection.h"
#include "image_processing.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>

// converti (x,y) en index linéaire dans un tableau 1D
#define IDX(x,y,w) ((y)*(w)+(x))

// Fonction récursive de remplissage (flood fill) pour trouver un contour/zone noire
static void flood_fill(ImageGray *img, int x,int y,bool *visited,int *minx,int *miny,int *maxx,int *maxy){
    int w = img->w, h = img->h;
    // condition de sortie : si on est hors de l'image, on revient
    if(x<0||y<0||x>=w||y>=h) return;
    int idx = IDX(x,y,w);
    //si déjà visité ou si le pixel est clair (>128) on ignore
    if(visited[idx] || img->pixels[idx]>128) return;
    // Marque comme visité
    visited[idx] = true;
    // Met à jour les bornes de la boîte englobante
    if(x<*minx) *minx=x;
    if(y<*miny) *miny=y;
    if(x>*maxx) *maxx=x;
    if(y>*maxy) *maxy=y;
    // appel récursif sur les 4 contours  : droite, gauche, bas, haut
    flood_fill(img,x+1,y,visited,minx,miny,maxx,maxy);
    flood_fill(img,x-1,y,visited,minx,miny,maxx,maxy);
    flood_fill(img,x,y+1,visited,minx,miny,maxx,maxy);
    flood_fill(img,x,y-1,visited,minx,miny,maxx,maxy);
}

// But principale : parcourt l'image et trouve toutes les zones noires (contours)
// remplit out_boxes avec un tableau de BoundingBox et count avec le nombre trouvé
int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count){
    if(!img || !img->pixels) return -1; // Vérification des pointeurs
    int w = img->w, h = img->h;
    // Tableau pour marquer les pixels déjà visités
    bool *visited = calloc(w*h,sizeof(bool));
    // Allocation initiale pour les boxes ; ici on réserve pour 10000 éléments, a agrandir si besoin de +
    BoundingBox *boxes = malloc(sizeof(BoundingBox)*10000);
    int n = 0;

    // Parcours ligne par ligne
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            int idx = IDX(x,y,w);
            // Si déjà visité ou pixel clair (>128) : on passe
            if(visited[idx] || img->pixels[idx]>128) continue;
            // Si on arrive ici, on a trouvé un pixel noir non visité -> commence un flood_fill
            int minx=x,miny=y,maxx=x,maxy=y;
            flood_fill(img,x,y,visited,&minx,&miny,&maxx,&maxy);
            // On calcule largeur et hauteur de la boîte détectée
            int bw = maxx-minx+1;
            int bh = maxy-miny+1;
            // Filtre de petites taches ou objets trop gros (bruit ou bords potentiels)
            if(bw>3 && bh>3 && bw<w/2 && bh<h/2){
                // On stocke la bounding box
                boxes[n++] = (BoundingBox){minx,miny,bw,bh};
                if(n>=9999) break; // sécurité pour ne pas dépasser l'allocation
            }
        }
    }

    // Libère le tableau de visited
    free(visited);
    // Renvoie le tableau et le compte
    *out_boxes = boxes;
    *count = n;//n'a pas le bon compte pour l'instant (les minis dessins sont compris)
    return 0;
}

// Fonction pour le visuel :dessine les boxes via un renderer SDL (rouge)
void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count){
    SDL_SetRenderDrawColor(ren,255,0,0,255); // rouge 
    for(int i=0;i<count;i++){
        SDL_Rect r = {boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h};
        SDL_RenderDrawRect(ren,&r); // dessine un rectangle
    }
}

