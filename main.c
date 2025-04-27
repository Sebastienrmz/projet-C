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
