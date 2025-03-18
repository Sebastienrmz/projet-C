#include "bmp8.h"
#include <stdio.h>

t_bmp8 * bmp8_loadImage(const char * filename){
    FILE* fd = fopen(filename, "rb");
    if (fd == NULL){
      return NULL;
    }
    t_bmp8 * mybtm = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (mybtm == NULL){
      return NULL;
    }

    mybtm->bmpHeader = ;
    mybtm->bmpColorTable = ;



}