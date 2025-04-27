#include <stdio.h>
#include "bmp8.h"  // Assure-toi que ce fichier est bien inclus

int main() {

    t_bmp8 *img = bmp8_loadImage("lena_gray.bmp");
    if (img == NULL) {
        printf("Erreur de chargement de l'image.\n");
        return 1;
    }

    bmp8_printInfo(img);

    bmp8_saveImage("copie_lena.bmp", img);
    printf("Image sauvegardée sous 'copie_lena.bmp'\n");

    bmp8_free(img);

    return 0;
}




#include <stdio.h>
#include "bmp8.h"

int main(void) {

    t_bmp8 *img = bmp8_loadImage("lena_gray.bmp");
    if (!img) {
        fprintf(stderr, "Échec du chargement BMP8\n");
        return 1;
    }


    bmp8_printInfo(img);


    bmp8_saveImage("lena_gray_copy.bmp", img);


    bmp8_free(img);

    return 0;
}




int main(void) {

    t_bmp24 *img = bmp24_loadImage("photo24.bmp");
    if (!img) {
        fprintf(stderr, "Échec du chargement BMP24\n");
        return 1;
    }


    t_pixel p00 = img->data[0][0];
    printf("Pixel (0,0) = B:%u G:%u R:%u\n",
           p00.blue, p00.green, p00.red);

    int cx = img->width/2, cy = img->height/2;
    t_pixel pc = img->data[cy][cx];
    printf("Pixel centre (%d,%d) = B:%u G:%u R:%u\n",
           cx, cy, pc.blue, pc.green, pc.red);


    bmp24_saveImage(img, "photo24_copy.bmp");


    bmp24_free(img);

    return 0;