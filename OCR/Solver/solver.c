#include <stdio.h>
#include <stdlib.h>
#include <err.h>

char **convert(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        errx(EXIT_FAILURE, "Fichier introuvable.");

    int nb_colonnes = 0;
    int nb_lignes = 0;
    int first_line = 1;
    char c;

    //Trouver nb_colonnes et nb_lignes
    while ((c = fgetc(f)) != EOF)
    {
        if (first_line && c != '\n')
            nb_colonnes++;
        else if (first_line && c == '\n')
        {
            nb_lignes++;
            first_line = 0;
        }
        else if (c == '\n')
            nb_lignes++;
    }
    nb_colonnes++;
    
    rewind(f); // Revenir au début du fichier

    // Creation de la grille avec malloc
    char **grid = malloc((nb_lignes + 1) * sizeof(char *));
    if (grid == NULL)
        errx(EXIT_FAILURE, "Gros fail Malloc");

    for (int i = 0; i < nb_lignes; i++)
    {
        grid[i] = malloc((nb_colonnes + 1) * sizeof(char));
        if (grid[i] == NULL)
            errx(EXIT_FAILURE, "Gros fail Malloc");
    }

    // Remplir la grille
    for (int i = 0; i < nb_lignes; i++)
    {
        for (int j = 0; j < nb_colonnes; j++)
        {
            int c;
            c = fgetc(f);
            if(c == EOF){
                break;
            }
            if(c!=' ' && c != '\n'){
                grid[i][j] = c;
            }
        }
    }

    fclose(f);

    return grid;
}

int *search(char **grid, char *name) 
{    
    int *result = malloc(4 * sizeof(int));
    if (result == NULL) 
    {
        err(1, "Gros fail sur Malloc");
    }

    result[0] = -1; result[1] = -1; result[2] = -1; result[3] = -1;

    //Differents couples pour les directions (0,1) (0,-1)...
    int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
    int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};

    //Calcul de la taille de la grille
    int rows = 0;
    while (grid[rows] != NULL) rows++;
    if (rows == 0) return result;

    int cols = 0;
    while (grid[0][cols] != '\0') cols++;

    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            for (int dir = 0; dir < 8; dir++) 
            {
                int k = 0;
                int x = i, y = j;

                while (name[k] != '\0') 
                {
                    if (x < 0 || x >= rows || y < 0 || y >= cols) break;
                    if (grid[x][y] != name[k]) break;

                    k++;
                    x += dx[dir];
                    y += dy[dir];
                }

                if (name[k] == '\0') 
                {
                    result[0] = j;
                    result[1] = i;
                    result[2] = j + (k-1) * dy[dir];  
                    result[3] = i + (k-1) * dx[dir];
                    return result;
                }
            }
        }
    }

    return result;
}

int main(int argc, char *argv[])
{
    char **grille = convert(argv[1]);

    int *coords;
    coords = search(grille, argv[2]);
    if(coords[0] == -1){
        printf("Not Found.\n");
    }
    else printf("Trouvé en (%d, %d) jusque (%d, %d)\n", coords[0], coords[1], coords[2], coords[3]);

    for (int i = 0; grille[i] != NULL; i++)
        free(grille[i]);
    free(grille);
    free(coords);

    return 0;
}

