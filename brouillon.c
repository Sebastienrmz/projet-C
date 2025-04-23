#include <stdio.h>
#include <stdlib.h>
#include <string.h>

t_bmp8 * bmp8_loadImage(const char * filename){

            FILE * fp;             //j'ouvre le fichier en binaire
            fp = fopen(filename, "rb");

            if(fp == NULL){      //je verifie si le fichier s'ouvre bien
                printf("Error opening file\n");
                return NULL;
                }

            t_bmp8 * mybtm = (t_bmp8*)malloc(sizeof(t_bmp8)); //j'alloue de la mémoire pour ma structure bmp et verifie si pas d'erreur
            if(mybtm == NULL){
                printf("Error allocating memory\n");
                return NULL;
                }

            fread(&(mybtm->header), sizeof(unsigned char), 54, fp); //je li 54 octets de l’en-tête d’un fichier bmp

            mybtm -> width = *(unsigned int*)(mybtm->header + 18);
            mybtm -> height = *(unsigned int*)(mybtm->header + 22);
            mybtm -> colorDepth = *(unsigned short*)(mybtm->header + 28);
            mybtm -> dataSize = *(unsigned int*)(mybtm->header + 34);

            if(mybtm->colorDepth == 8){
                mybtm->data = (unsigned char *)malloc(mybtm->dataSize);
                if(mybtm->data == NULL){
                    printf("Error allocating memory for image data\n");
                    free(mybtm); // on libère l’image si data échoue
                    return NULL;
                }
                fread(mybtm->data, 1, mybtm->dataSize, fp);
            }
            fclose(fp);
            return mybtm;
            }




void bmp8_saveImage(const char * filename, t_bmp8 * img) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    // Écrire l'en-tête BMP (54 octets)
    fwrite(img->header, sizeof(unsigned char), 54, fp);

    // Écrire la table de couleurs si profondeur = 8 bits (1024 octets)
    if (img->colorDepth == 8) {
        fwrite(img->colorTable, sizeof(unsigned char), 1024, fp);
    }

    // Écrire les données de l'image (dataSize octets)
    fwrite(img->data, sizeof(unsigned char), img->dataSize, fp);

    fclose(fp);
}