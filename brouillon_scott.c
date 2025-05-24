#include <stdio.h>

#include <stdio.h>
#include <errno.h>

int main() {
    int choix;

    while (1) {
        // Affichage du menu principal
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Appliquer des filtres sur une image 8 bits (barbara_gray.bmp)\n");
        printf("2. Appliquer des filtres sur une image 24 bits (flowers_color.bmp)\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        if (choix == 0) break;

        if (choix == 1) {
            // Traitement pour image 8 bits
            t_bmp8 *img8 = bmp8_loadImage("../barbara_gray.bmp");
            if (!img8) {
                printf("Erreur : image 8 bits introuvable\n");
                continue;
            }

            bmp8_printInfo(img8);

            // Application du filtre négatif
            bmp8_negative(img8);
            bmp8_saveImage(img8, "barbara_gray_negative.bmp");

            // Augmentation de la luminosité
            bmp8_brightness(img8, 50);
            bmp8_saveImage(img8, "barbara_gray_brightness.bmp");

            // Seuil
            bmp8_threshold(img8, 100);
            bmp8_saveImage(img8, "barbara_gray_threshold.bmp");

            bmp8_free(img8);
            printf("✔️ Filtres 8 bits appliqués avec succès\n");

        } else if (choix == 2) {
            t_bmp24 *img24;

            // === Filtre Négatif ===
            img24 = bmp24_loadImage("../flowers_color.bmp");
            if (!img24) {
                printf("Erreur : image 24 bits introuvable\n");
                continue;
            }
            bmp24_negative(img24);
            bmp24_saveImage(img24, "flowers_negative.bmp");
            bmp24_free(img24);

            // === Filtre Niveaux de gris ===
            img24 = bmp24_loadImage("../flowers_color.bmp");
            if (!img24) {
                printf("Erreur : image 24 bits introuvable\n");
                continue;
            }
            bmp24_grayscale(img24);
            bmp24_saveImage(img24, "flowers_grayscale.bmp");
            bmp24_free(img24);

            // === Filtre Luminosité ===
            img24 = bmp24_loadImage("../flowers_color.bmp");
            if (!img24) {
                printf("Erreur : image 24 bits introuvable\n");
                continue;
            }
            bmp24_brightness(img24, 50);
            bmp24_saveImage(img24, "flowers_brightness.bmp");
            bmp24_free(img24);

            printf("✔️ Filtres 24 bits appliqués avec succès\n");

        } else {
            printf("❌ Choix invalide. Veuillez réessayer.\n");
        }
    }

    printf("👋 Au revoir !\n");
    return 0;
}

