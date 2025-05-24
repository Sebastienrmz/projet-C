#include <stdio.h>

t_bmp8 *bmp8_loadImage(const char *filename) {
    // Ouverture du fichier en mode binaire
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Erreur : impossible d’ouvrir le fichier\n");
        return NULL;
    }

    // Allocation mémoire pour la structure t_bmp8
    t_bmp8 *image = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!image) {
        printf("Erreur : échec de l’allocation mémoire\n");
        fclose(fp);
        return NULL;
    }

    // Lecture des 54 octets de l’en-tête BMP
    fread(image->header, sizeof(unsigned char), 54, fp);

    // Extraction des informations depuis l’en-tête
    image->width       = *(unsigned int *)(image->header + 18);
    image->height      = *(unsigned int *)(image->header + 22);
    image->colorDepth  = *(unsigned short *)(image->header + 28);
    image->dataSize    = *(unsigned int *)(image->header + 34);

    // Vérifie que l’image est bien en 8 bits
    if (image->colorDepth != 8) {
        printf("Profondeur de couleur non supportée : %u\n", image->colorDepth);
        free(image);
        fclose(fp);
        return NULL;
    }

    // Lecture de la table des couleurs (256 entrées de 4 octets)
    fread(image->colorTable, sizeof(unsigned char), 1024, fp);

    // Si dataSize vaut 0, on le calcule manuellement
    if (image->dataSize == 0) {
        image->dataSize = image->width * image->height;
    }

    // Allocation mémoire pour les données de l’image
    image->data = (unsigned char *)malloc(image->dataSize);
    if (!image->data) {
        printf("Erreur : échec de l’allocation mémoire pour les données image\n");
        free(image);
        fclose(fp);
        return NULL;
    }

    // Lecture des données de l’image
    fread(image->data, 1, image->dataSize, fp);

    fclose(fp);
    return image;
}


