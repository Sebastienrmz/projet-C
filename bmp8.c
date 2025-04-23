#include "bmp8.h"
#include <stdio.h>

t_bmp8 * bmp8_loadImage(const char * filename){
    FILE* fd = fopen(filename, "rb");
    if (fd == NULL){
        printf("Erreur : Impossible d'ouvrir le fichier BMP.\n");
        return NULL;
    }
    t_bmp8 * mybtm = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (mybtm == NULL){
        printf("Erreur : Allocation mémoire échouée.\n");
        return NULL;
    }}

fread(&(mybtm->bmpHeader), sizeof(BMPHeader), 1, fd);
fread(&(mybtm->dibHeader), sizeof(DIBHeader), 1, fd);



