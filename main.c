#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

void afficherMenuPrincipal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Filtres 8 bits (barbara_gray.bmp)\n");
    printf("2. Filtres 24 bits (flowers_color.bmp)\n");
    printf("3. Égalisation d'histogramme (8 bits uniquement)\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
}

void afficherMenuFiltres8() {
    printf("\n-- Filtres 8 bits --\n");
    printf("1. Négatif\n");
    printf("2. Luminosité (valeur entre -255 et 255)\n");
    printf("3. Seuillage (valeur entre 0 et 255)\n");
    printf("0. Retour\n");
    printf("Votre choix : ");
}

void afficherMenuFiltres24() {
    printf("\n-- Filtres 24 bits --\n");
    printf("1. Négatif\n");
    printf("2. Niveau de gris\n");
    printf("3. Luminosité (valeur entre -255 et 255)\n");
    printf("0. Retour\n");
    printf("Votre choix : ");
}

int main() {
    int choix;

    while (1) {
        afficherMenuPrincipal();
        scanf("%d", &choix);

        if (choix == 0) break;

        if (choix == 1) {
            t_bmp8 *img8 = bmp8_loadImage("../barbara_gray.bmp");
            if (!img8) {
                printf("Erreur : image 8 bits introuvable\n");
                continue;
            }

            bmp8_printInfo(img8);
            int filtre;
            do {
                afficherMenuFiltres8();
                scanf("%d", &filtre);
                switch (filtre) {
                    case 1:
                        bmp8_negative(img8);
                        bmp8_saveImage(img8, "barbara_gray_negative.bmp");
                        printf("✔️ Négatif appliqué\n");
                        break;
                    case 2: {
                        int val;
                        printf("Valeur de luminosité [-255 ; 255] : ");
                        scanf("%d", &val);
                        if (val < -255) val = -255;
                        if (val > 255) val = 255;
                        bmp8_brightness(img8, val);
                        bmp8_saveImage(img8, "barbara_gray_brightness.bmp");
                        printf("✔️ Luminosité appliquée\n");
                        break;
                    }
                    case 3: {
                        int seuil;
                        printf("Valeur de seuillage [0 ; 255] : ");
                        scanf("%d", &seuil);
                        if (seuil < 0) seuil = 0;
                        if (seuil > 255) seuil = 255;
                        bmp8_threshold(img8, seuil);
                        bmp8_saveImage(img8, "barbara_gray_threshold.bmp");
                        printf("✔️ Seuillage appliqué\n");
                        break;
                    }
                    case 0:
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
            } while (filtre != 0);
            bmp8_free(img8);

        } else if (choix == 2) {
            int filtre;
            do {
                afficherMenuFiltres24();
                scanf("%d", &filtre);
                t_bmp24 *img24 = bmp24_loadImage("../flowers_color.bmp");
                if (!img24) {
                    printf("Erreur : image 24 bits introuvable\n");
                    break;
                }

                switch (filtre) {
                    case 1:
                        bmp24_negative(img24);
                        bmp24_saveImage(img24, "flowers_negative.bmp");
                        printf("✔️ Négatif appliqué\n");
                        break;
                    case 2:
                        bmp24_grayscale(img24);
                        bmp24_saveImage(img24, "flowers_grayscale.bmp");
                        printf("✔️ Niveau de gris appliqué\n");
                        break;
                    case 3: {
                        int val;
                        printf("Valeur de luminosité [-255 ; 255] : ");
                        scanf("%d", &val);
                        if (val < -255) val = -255;
                        if (val > 255) val = 255;
                        bmp24_brightness(img24, val);
                        bmp24_saveImage(img24, "flowers_brightness.bmp");
                        printf("✔️ Luminosité appliquée\n");
                        break;
                    }
                    case 0:
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
                bmp24_free(img24);
            } while (filtre != 0);

        } else if (choix == 3) {
            t_bmp8 *img = bmp8_loadImage("../barbara_gray.bmp");
            if (!img) {
                printf("Erreur : image 8 bits introuvable\n");
                continue;
            }

            bmp8_saveImage(img, "result_original.bmp");

            unsigned int *hist = bmp8_computeHistogram(img);
            unsigned int *cdf = bmp8_computeCDF(hist);
            bmp8_equalize(img, cdf);
            bmp8_saveImage(img, "result_equalized.bmp");

            bmp8_free(img);
            free(hist);
            free(cdf);

            printf("✔️ Égalisation d’histogramme terminée\n");

        } else {
            printf("Choix invalide. Réessayez.\n");
        }
    }

    printf("Fin du programme. ✅\n");
    return 0;
}
