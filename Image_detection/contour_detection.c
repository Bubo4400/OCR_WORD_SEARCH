#include "contour_detection.h"
#include "image_processing.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
// convert (x,y) to linear index
#define IDX(x,y,w) ((y)*(w)+(x))
/*Flood-fill : expand a connected region (4-neighbors)
  - visited: boolean array of size w*h marking already-visited pixels
  - Updates minx, miny, maxx, maxy to the bounding box of the region.*/

static void flood_fill(ImageGray *img, int x,int y,bool *visited,int *minx,int *miny,int *maxx,int *maxy){
    int w = img->w, h = img->h;
    /* bounds check */
    if( x < 0 || y <0 ||x>=w||y>=h) return;
    int idx = IDX(x,y,w);

    /* skip if already visited or pixel is background (light) */
    if(visited[idx] || img->pixels[idx]>128) return;
    /* mark visited and update bbox */
    visited[idx] = true;

    if(x<*minx) *minx=x;
    if(y<*miny) *miny=y;
    if(x>*maxx) *maxx=x;
    if(y>*maxy) *maxy=y;

    /* recursively expand to 4-connected neighbors */
    flood_fill(img,x+1,y,visited,minx,miny,maxx,maxy);
    flood_fill(img,x-1,y,visited,minx,miny,maxx,maxy);
    flood_fill(img,x,y+1,visited,minx,miny,maxx,maxy);
    flood_fill(img,x,y-1,visited,minx,miny,maxx,maxy);
}
/* find_contours
  - Allocate visited[] (initialized to false)
  - Iterate every pixel; when an unvisited dark pixel is found, flood-fill it.
  - Compute bounding box and apply simple size filters to reject noise.
  - Append bounding boxes to an allocated array (caller must free).
 */
int find_contours(ImageGray *img, BoundingBox **out_boxes, int *count){
    if(!img || !img->pixels) return -1;
    int w = img->w, h = img->h;
    bool *visited =  calloc(w*h,sizeof(bool));
    BoundingBox *boxes  = malloc(sizeof(BoundingBox)*10000);
    int n = 0;

    for(int y = 0; y < h ;y++){
        for(int x=0;x<w;x++){
            int idx = IDX(x,y,w);
            if( visited[idx] || img->pixels[idx]>128) continue;
            int minx=x,miny=y,maxx=x,maxy=y;
            flood_fill(img,x,y,visited,&minx,&miny,&maxx,&maxy);
            int bw = maxx-minx+1;
            int bh = maxy-miny+1;
            if(bw>3 && bh>3 && bw<w/2 && bh<h/2){
                boxes[n++] = (BoundingBox){minx,miny,bw,bh};
                if(n>=9999) break;
            }
        }
    }

    free(visited);
    *out_boxes = boxes;
    *count = n;
    return 0;
}

void draw_contours(SDL_Renderer *ren, BoundingBox *boxes, int count){
    SDL_SetRenderDrawColor(ren,255,0,0,255);
    for(int i=0;i<count;i++){
        SDL_Rect r = {boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h};
        SDL_RenderDrawRect(ren,&r);
    }
}

