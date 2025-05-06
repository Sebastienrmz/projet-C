#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

// Création d'un noyau box blur 3x3
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
    for (int i = 0; i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

int main() {
    // Charger l'image (adapter le chemin si nécessaire)
    t_bmp8 *img = bmp8_loadImage("../lena_gray.bmp");

    if (img == NULL) {
        printf("Erreur : impossible de charger lena_gray.bmp\n");
        return 1;
    }

    // Sauvegarde de l'image originale
    bmp8_saveImage(img, "../result_original.bmp");

    // ----------------------------
    // Appliquer un filtre box blur
    // ----------------------------
    float **kernel = createBoxBlurKernel();
    bmp8_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
    bmp8_saveImage(img, "result_blur.bmp");

    // ----------------------------
    // Appliquer une inversion
    // ----------------------------
    bmp8_negative(img);
    bmp8_saveImage(img, "result_negative.bmp");

    // ----------------------------
    // Appliquer un éclaircissement
    // ----------------------------
    bmp8_brightness(img, 50);
    bmp8_saveImage(img, "result_brightness.bmp");

    // ----------------------------
    // Appliquer un seuillage
    // ----------------------------
    bmp8_threshold(img, 128);
    bmp8_saveImage(img, "result_threshold.bmp");

    // Libération mémoire
    bmp8_free(img);
    return 0;
}
