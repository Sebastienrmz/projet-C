#include <stdio.h>
#include "bmp8.h"  // Assure-toi que ce fichier est bien inclus

int main() {
    // Chargement de l’image BMP (assure-toi que "lena_gray.bmp" est dans le dossier du projet)
    t_bmp8 *img = bmp8_loadImage("lena_gray.bmp");
    if (img == NULL) {
        printf("Erreur de chargement de l'image.\n");
        return 1;
    }

    // Affiche les informations de l’image
    bmp8_printInfo(img);

    // Sauvegarde l’image sous un nouveau nom
    bmp8_saveImage("copie_lena.bmp", img);
    printf("Image sauvegardée sous 'copie_lena.bmp'\n");

    // Libération mémoire
    bmp8_free(img);

    return 0;
}
