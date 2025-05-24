#include <stdio.h>

#include <stdio.h>
#include <errno.h>

void bmp8_saveImage(t_bmp8 *img, const char *filename) {
    // Ouvre le fichier en écriture binaire
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Erreur : impossible d’ouvrir le fichier en écriture\n");
        printf("fopen a échoué, errno = %d\n", errno);
        return;
    }

    // Écrit l'en-tête BMP (54 octets)
    fwrite(img->header, sizeof(unsigned char), 54, fp);

    // Si l'image est en 8 bits, écrire la table des couleurs (256 x 4 octets = 1024)
    if (img->colorDepth == 8) {
        fwrite(img->colorTable, sizeof(unsigned char), 1024, fp);
    }

    // Écrit les données de l'image
    fwrite(img->data, sizeof(unsigned char), img->dataSize, fp);

    // Ferme le fichier
    fclose(fp);
}


