#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

int main() {
    char inputPath[] = "../barbara_gray.bmp";
    t_bmp8 *img = bmp8_loadImage(inputPath);
    if (img == NULL) return 1;

    bmp8_printInfo(img);

    int choix;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Négatif\n");
        printf("2. Modifier la luminosité\n");
        printf("3. Appliquer un seuillage\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                bmp8_negative(img);
                bmp8_saveImage(img, "barbara_gray_negative.bmp");
                printf("✅ Image enregistrée : barbara_gray_negative.bmp\n");
                break;

            case 2: {
                int value;
                printf("Entrez la valeur de luminosité (-255 à +255) : ");
                scanf("%d", &value);
                bmp8_brightness(img, value);
                bmp8_saveImage(img, "barbara_gray_brightness.bmp");
                printf("✅ Image enregistrée : barbara_gray_brightness.bmp\n");
                break;
            }

            case 3: {
                int threshold;
                printf("Entrez le seuil (0 à 255) : ");
                scanf("%d", &threshold);
                bmp8_threshold(img, threshold);
                bmp8_saveImage(img, "barbara_gray_threshold.bmp");
                printf("✅ Image enregistrée : barbara_gray_threshold.bmp\n");
                break;
            }

            case 0:
                printf("Fermeture du programme.\n");
                break;

            default:
                printf("❌ Choix invalide.\n");
        }

    } while (choix != 0);

    bmp8_free(img);
    return 0;
}
