#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"


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

    mybtm -> width = *(unsigned int*)(mybtm->header + 18);  //on va chercher les valeur dans le header a leur place pour les mettre a leur place
    mybtm -> height = *(unsigned int*)(mybtm->header + 22);
    mybtm -> colorDepth = *(unsigned short*)(mybtm->header + 28);
    mybtm -> dataSize = *(unsigned int*)(mybtm->header + 34);

    if(mybtm->colorDepth == 8){  	// on lit les données et on les stock dans data
    mybtm->data = (unsigned char *)malloc(mybtm->dataSize);
    if(mybtm->data == NULL){
    	printf("Error allocating memory for image data\n");
        free(mybtm);
        return NULL;
        }
    fread(mybtm->data, 1, mybtm->dataSize, fp); //fread(Adresse où stocker les données lues, taille de un élement, nb element a lire, fichier ouvert);
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

    fwrite(img->header, sizeof(unsigned char), 54, fp);  //fwrite(adresse, taille_d_un_element, nombre_d_elements, fichier);

    if (img->colorDepth == 8) {
        fwrite(img->colorTable, sizeof(unsigned char), 1024, fp);
    }

    fwrite(img->data, sizeof(unsigned char), img->dataSize, fp);  //fwrite(adresse, taille_d_un_element, nombre_d_elements, fichier);

    fclose(fp);
}




void bmp8_free(t_bmp8 * img) {
	if (img == NULL){     //on verifie si img pointevers quelque chose
		printf("Error in bmp8_free\n");
		return;
	}
	free(img->data);  //on free d'abord cela pour pas perdre l'adresse du data
	free(img);
	}//enfin on free le img global




void bmp8_printInfo(t_bmp8 * img) {
	printf("Image Info:\n");    //on va chercher chaque valeur grace au pointeur img qui pointe vers la struct t_bmp8
	printf("   Width: %d \n", img->width);
	printf("   Height: %d \n", img->height);
	printf("   Colors Depth: %d \n", img->colorDepth);
	printf("   Data Size: %d \n", img->dataSize);
}




void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}




void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int new_value = img->data[i] + value;
        if (new_value < 0) {
            new_value = 0;
        }
        if (new_value > 255) {
            new_value = 255;
        }
        img->data[i] = (unsigned char)new_value;
    }
}




void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        if (img->data[i] > threshold) {
            img->data[i] = 255;
        } else {
            img->data[i] = 0;
        }
    }
}
