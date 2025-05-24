#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

int main() {
    int choix;

    while (1) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Filtres 8 bits (barbara_gray.bmp)\n");
        printf("2. Filtres 24 bits (flowers_color.bmp)\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        if (choix == 0) break;

        if (choix == 1) {
            t_bmp8 *img8 = bmp8_loadImage("../barbara_gray.bmp");
            if (!img8) {
                printf("Erreur : image 8 bits introuvable\n");
                continue;
            }

            bmp8_printInfo(img8);

            bmp8_negative(img8);
            bmp8_saveImage(img8, "barbara_gray_negative.bmp");

            bmp8_brightness(img8, 50);
            bmp8_saveImage(img8, "barbara_gray_brightness.bmp");

            bmp8_threshold(img8, 100);
            bmp8_saveImage(img8, "barbara_gray_threshold.bmp");

            bmp8_free(img8);
            printf("✔️ Filtres 8 bits appliqués\n");

        } else if (choix == 2) {
            t_bmp24 *img24;

            // Négatif
            img24 = bmp24_loadImage("../flowers_color.bmp");
            if (img24 == NULL) {
                printf("Erreur : chargement image 24 bits a échoué\n");
                return 1;
            }
            if (!img24) {
                printf("Erreur : image 24 bits introuvable\n");
                continue;
            }
            bmp24_negative(img24);
            bmp24_saveImage(img24, "flowers_negative.bmp");
            bmp24_free(img24);

            // Niveaux de grissssss
            img24 = bmp24_loadImage("../flowers_color.bmp");
            bmp24_grayscale(img24);
            bmp24_saveImage(img24, "flowers_grayscale.bmp");
            bmp24_free(img24);

            // Luminosité
            img24 = bmp24_loadImage("../flowers_color.bmp");
            bmp24_brightness(img24, 50);
            bmp24_saveImage(img24, "flowers_brightness.bmp");
            bmp24_free(img24);

            printf("✔️ Filtres 24 bits appliqués\n");

        } else {
            printf("Choix invalide.\n");
        }
    }

    printf("Au revoir !\n");
    return 0;
}
