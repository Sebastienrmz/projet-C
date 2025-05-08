#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

// Création d’un noyau box blur 3x3
float **createBoxBlurKernel() {
    int size = 3;
    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    return kernel;
}

void freeKernel(float **kernel, int size) {
    for (int i = 0; i < size; i++)
        free(kernel[i]);
    free(kernel);
}

int main() {
    t_bmp8 *img = NULL;
    int choix, value, threshold;
    char inputFile[100];

    printf("Nom de l'image BMP (ex: lena_gray.bmp) : ");
    scanf("%s", inputFile);

    img = bmp8_loadImage(inputFile);
    if (!img) {
        printf("Erreur : impossible de charger l'image.\n");
        return 1;
    }

    // Sauvegarde de l'image originale
    bmp8_saveImage(img, "result_original.bmp");
    printf("Image originale sauvegardee sous 'result_original.bmp'\n");

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Appliquer un filtre negatif\n");
        printf("2. Modifier la luminosite\n");
        printf("3. Appliquer un seuillage\n");
        printf("4. Appliquer un flou (box blur)\n");
        printf("5. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                bmp8_negative(img);
                bmp8_saveImage(img, "result_negatif.bmp");
                printf("Sauvegarde : result_negatif.bmp\n");
                break;

            case 2:
                printf("Valeur de luminosite (ex: +50 ou -30) : ");
                scanf("%d", &value);
                bmp8_brightness(img, value);
                bmp8_saveImage(img, "result_brightness.bmp");
                printf("Sauvegarde : result_brightness.bmp\n");
                break;

            case 3:
                printf("Seuil (0 à 255) : ");
                scanf("%d", &threshold);
                bmp8_threshold(img, threshold);
                bmp8_saveImage(img, "result_threshold.bmp");
                printf("Sauvegarde : result_threshold.bmp\n");
                break;

            case 4: {
                float **kernel = createBoxBlurKernel();
                bmp8_applyFilter(img, kernel, 3);
                bmp8_saveImage(img, "result_blur.bmp");
                printf("Sauvegarde : result_blur.bmp\n");
                freeKernel(kernel, 3);
                break;
            }

            case 5:
                bmp8_free(img);
                printf("Fin du programme.\n");
                return 0;

            default:
                printf("Choix invalide. Veuillez reessayer.\n");
        }
    }

    return 0;
}
