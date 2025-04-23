#include <stdio.h>
#include "bmp8.h"  // ou le fichier où tu as défini les fonctions

int main() {
    t_bmp8 *img = bmp8_loadImage("image.bmp"); // Ton image ici

    if (img == NULL) {
        printf("Échec du chargement de l'image.\n");
        return 1;
    }

    bmp8_printInfo(img);  // ← TEST ICI 🎯

    bmp8_free(img);       // Toujours libérer la mémoire !
    return 0;
}


