#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"


#define MAX_PATH 512

void afficherMenuPrincipal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Filtres 8 bits\n");
    printf("2. Filtres 24 bits\n");
    printf("3. Egalisation d'histogramme (8 bits seulement)\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
}

void afficherMenuFiltres8() {
    printf("\n-- Filtres 8 bits --\n");
    printf("1. Negatif\n");
    printf("2. Luminosite (valeur entre -255 et 255)\n");
    printf("3. Seuillage (valeur entre 0 et 255)\n");
    printf("0. Retour\n");
    printf("Votre choix : ");
}

void afficherMenuFiltres24() {
    printf("\n-- Filtres 24 bits --\n");
    printf("1. Negatif\n");
    printf("2. Niveau de gris\n");
    printf("3. Luminosite (valeur entre -255 et 255)\n");
    printf("0. Retour\n");
    printf("Votre choix : ");
}

int main() {
    int choix;
    char path[MAX_PATH];

    while (1) {
        afficherMenuPrincipal();
        scanf("%d", &choix);
        getchar();

        if (choix == 0) break;

        if (choix == 1) {
            //--- Partie 8 bits ---
            printf("Chemin de l'image 8 bits : ");
            fgets(path, MAX_PATH, stdin);
            path[strcspn(path, "\n")] = '\0';

            t_bmp8 *img8 = bmp8_loadImage(path);
            if (!img8) {
                printf("Erreur : impossible de charger '%s'\n", path);
                continue;
            }

            bmp8_printInfo(img8);

            int filtre8;
            do {
                afficherMenuFiltres8();
                scanf("%d", &filtre8);
                switch (filtre8) {
                    case 1:
                        bmp8_negative(img8);
                        bmp8_saveImage(img8, "../result_8_negative.bmp");
                        printf(" Negatif appliqué\n");
                        break;
                    case 2: {
                        int val;
                        printf("Valeur de luminosite [-255 ; 255] : ");
                        scanf("%d", &val);
                        if (val < -255) val = -255;
                        if (val > 255) val = 255;
                        bmp8_brightness(img8, val);
                        bmp8_saveImage(img8, "../result_8_brightness.bmp");
                        printf(" Luminosite appliquee\n");
                        break;
                    }
                    case 3: {
                        int seuil;
                        printf("Valeur de seuillage [0 ; 255] : ");
                        scanf("%d", &seuil);
                        if (seuil < 0) seuil = 0;
                        if (seuil > 255) seuil = 255;
                        bmp8_threshold(img8, seuil);
                        bmp8_saveImage(img8, "../result_8_threshold.bmp");
                        printf(" Seuillage applique\n");
                        break;
                    }
                    case 0:
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
            } while (filtre8 != 0);

            bmp8_free(img8);

        } else if (choix == 2) {
            //--- Partie 24 bits ---
            printf("Chemin de l'image 24 bits : ");
            fgets(path, MAX_PATH, stdin);
            path[strcspn(path, "\n")] = '\0';

            t_bmp24 *img24 = bmp24_loadImage(path);
            if (!img24) {
                printf("Erreur : impossible de charger '%s'\n", path);
                continue;
            }

            int filtre24;
            do {
                afficherMenuFiltres24();
                scanf("%d", &filtre24);
                switch (filtre24) {
                    case 1:
                        bmp24_negative(img24);
                        bmp24_saveImage(img24, "../result_24_negative.bmp");
                        printf("Negatif applique\n");
                        break;
                    case 2:
                        bmp24_grayscale(img24);
                        bmp24_saveImage(img24, "../result_24_grayscale.bmp");
                        printf(" Niveau de gris applique\n");
                        break;
                    case 3: {
                        int val;
                        printf("Valeur de luminosite [-255 ; 255] : ");
                        scanf("%d", &val);
                        if (val < -255) val = -255;
                        if (val > 255) val = 255;
                        bmp24_brightness(img24, val);
                        bmp24_saveImage(img24, "../result_24_brightness.bmp");
                        printf(" Luminosite appliquee\n");
                        break;
                    }
                    case 0:
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
            } while (filtre24 != 0);

            bmp24_free(img24);

        } else if (choix == 3) {
            //--- Égalisation histogramme (8 bits) ---
            printf("Chemin de l'image 8 bits pour egalisation : ");
            fgets(path, MAX_PATH, stdin);
            path[strcspn(path, "\n")] = '\0';

            t_bmp8 *imgH = bmp8_loadImage(path);
            if (!imgH) {
                printf("Erreur : impossible de charger '%s'\n", path);
                continue;
            }

            unsigned int *hist = bmp8_computeHistogram(imgH);
            unsigned int *cdf  = bmp8_computeCDF(hist);
            bmp8_equalize(imgH, cdf);
            bmp8_saveImage(imgH, "../result_equalized.bmp");

            bmp8_free(imgH);
            free(hist);
            free(cdf);

            printf("Egalisation d histogramme terminee\n");

        } else {
            printf("Choix invalide. Réessayez.\n");
        }
    }

    printf("Fin du programme. \n");
    return 0;
}