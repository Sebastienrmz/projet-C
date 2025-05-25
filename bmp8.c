#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include <stdint.h>
#include <math.h>
t_bmp8 * bmp8_loadImage(const char * filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    t_bmp8 *mybtm = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (mybtm == NULL) {
        printf("Error allocating memory\n");
        fclose(fp);
        return NULL;
    }

    fread(mybtm->header, sizeof(unsigned char), 54, fp);

    mybtm->width = *(unsigned int *)(mybtm->header + 18);
    mybtm->height = *(unsigned int *)(mybtm->header + 22);
    mybtm->colorDepth = *(unsigned short *)(mybtm->header + 28);
    mybtm->dataSize = *(unsigned int *)(mybtm->header + 34);

    if (mybtm->colorDepth == 8) {
        fread(mybtm->colorTable, sizeof(unsigned char), 1024, fp);

        // Si dataSize vaut 0, on le calcule manuellement
        if (mybtm->dataSize == 0) {
            mybtm->dataSize = mybtm->width * mybtm->height;
        }

        mybtm->data = (unsigned char *)malloc(mybtm->dataSize);
        if (mybtm->data == NULL) {
            printf("Error allocating memory for image data\n");
            fclose(fp);
            free(mybtm);
            return NULL;
        }

        fread(mybtm->data, 1, mybtm->dataSize, fp);
    } else {
        printf("Unsupported color depth: %u\n", mybtm->colorDepth);
        fclose(fp);
        free(mybtm);
        return NULL;
    }

    fclose(fp);
    return mybtm;
}



void bmp8_saveImage(t_bmp8 * img, const char * filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file\n");
        printf("fopen failed, errno = %d\n", errno);
        return;
    }

    fwrite(img->header, sizeof(unsigned char), 54, fp);  //fwrite(adresse, taille_d_un_element, nombre_d_elements, fichier);

    if (img->colorDepth == 8) {
        fwrite(img->colorTable, sizeof(unsigned char), 1024, fp);
    }

    fwrite(img->data, sizeof(unsigned char), img->dataSize, fp);  //fwrite(adresse, taille_d_un_element, nombre_d_elements, fichier);

    fclose(fp);
}




void bmp8_free(t_bmp8 * img) {
	if (img == NULL){     //on verifie si img pointevers quelque chose
		printf("Error in bmp8_free\n");
		return;
	}
	free(img->data);  //on free d'abord cela pour pas perdre l'adresse du data
	free(img);
	}//enfin on free le img global




void bmp8_printInfo(t_bmp8 * img) {
	printf("Image Info:\n");    //on va chercher chaque valeur grace au pointeur img qui pointe vers la struct t_bmp8
	printf("   Width: %d \n", img->width);
	printf("   Height: %d \n", img->height);
	printf("   Colors Depth: %d \n", img->colorDepth);
	printf("   Data Size: %d \n", img->dataSize);
}




void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}




void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int new_value = img->data[i] + value;
        if (new_value < 0) {
            new_value = 0;
        }
        if (new_value > 255) {
            new_value = 255;
        }
        img->data[i] = (unsigned char)new_value;
    }
}




void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        if (img->data[i] > threshold) {
            img->data[i] = 255;
        } else {
            img->data[i] = 0;
        }
    }
}




void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL) {
        printf("Error: image or data is NULL\n");
        return;
    }

    int width = img->width;
    int height = img->height;
    int n = kernelSize / 2;

    //Allouer une copie temporaire pour stocker les nouveaux pixels
    unsigned char *tempData = malloc(img->dataSize);
    if (tempData == NULL) {
        printf("Error: failed to allocate memory for filtered image\n");
        return;
    }

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0.0;

            for (int i = -n; i <= n; i++) {
                for (int j = -n; j <= n; j++) {
                    int pixel = img->data[(y + i) * width + (x + j)];
                    float coeff = kernel[i + n][j + n];
                    sum += pixel * coeff;
                }
            }

            // Clamp entre 0 et 255
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            tempData[y * width + x] = (unsigned char)sum;
        }
    }

    //Copier le résultat dans l’image d’origine
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            img->data[y * width + x] = tempData[y * width + x];
        }
    }

    free(tempData);
}

















t_pixel ** bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) return NULL;

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {

            for (int j = 0; j < i; j++) free(pixels[j]);
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}




void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels == NULL) return;

    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}




t_bmp24 * bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) return NULL;

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->pixels = bmp24_allocateDataPixels(width, height);
    if (img->pixels == NULL) {
        free(img);
        return NULL;
    }

    return img;
}




void bmp24_free(t_bmp24 *img) {
    if (img == NULL) return;
    bmp24_freeDataPixels(img->pixels, img->height);
    free(img);
}





t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête BMP (54 octets)
    file_rawRead(0, img->header, sizeof(uint8_t), 54, file);

    // Lecture des informations supplémentaires
    file_rawRead(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    img->pixels = bmp24_allocateDataPixels(img->width, img->height);
    if (img->pixels == NULL) {
        free(img);
        fclose(file);
        return NULL;
    }

    // Récupération de l'offset des données pixels
    uint32_t offset = *(uint32_t *)(img->header + BITMAP_OFFSET);

    // Lecture des pixels en partant du bas de l'image (format BMP)
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t color[3];
            uint32_t pos = offset + ((img->height - 1 - y) * img->width + x) * 3;
            file_rawRead(pos, color, sizeof(uint8_t), 3, file);

            img->pixels[y][x].blue = color[0];
            img->pixels[y][x].green = color[1];
            img->pixels[y][x].red = color[2];
        }
    }

    fclose(file);
    return img;
}






void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    // On tente d'ouvrir le fichier en mode écriture binaire
    FILE *file = fopen(filename, "wb");
    if (!file) {
        // Si l'ouverture échoue, on affiche un message d'erreur
        printf("Erreur : impossible de créer le fichier %s\n", filename);
        return;
    }

    // On écrit d'abord l'en-tête BMP standard (54 octets)
    file_rawWrite(0, img->header, sizeof(uint8_t), 54, file);

    // Ensuite on écrit les informations additionnelles (dimensions, profondeur de couleur, etc.)
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // On récupère la position dans le fichier où commencent réellement les données d'image
    uint32_t offset = *(uint32_t *)(img->header + BITMAP_OFFSET);

    // On parcourt tous les pixels de l'image ligne par ligne
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Préparation des trois composantes de couleur dans l'ordre BGR
            uint8_t color[3];
            color[0] = img->pixels[y][x].blue;
            color[1] = img->pixels[y][x].green;
            color[2] = img->pixels[y][x].red;

            // On calcule la position exacte où écrire ce pixel dans le fichier
            uint32_t pos = offset + ((img->height - 1 - y) * img->width + x) * 3;

            // Et on écrit les 3 octets (1 pour chaque composante) au bon endroit
            file_rawWrite(pos, color, sizeof(uint8_t), 3, file);
        }
    }

    // Une fois terminé, on ferme proprement le fichier
    fclose(file);
}











void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    // On place le curseur du fichier à la position souhaitée (en octets depuis le début)
    fseek(file, position, SEEK_SET);

    // Puis on lit 'n' éléments de taille 'size' chacun à partir de cette position,
    // et on les stocke dans le buffer fourni
    fread(buffer, size, n, file);
}



void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    // Calcul de la position du pixel dans le fichier BMP.
    // On part de l'offset de l'image (BITMAP_OFFSET), puis on ajoute l'index du pixel dans le tableau.
    // L'image BMP étant stockée à l’envers (de bas en haut), on inverse la ligne : (height - 1 - y).
    uint32_t position = BITMAP_OFFSET + ((image->height - 1 - y) * image->width + x) * 3;

    // Tableau pour stocker temporairement les 3 composantes couleur (bleu, vert, rouge)
    uint8_t color[3];

    // Lecture des 3 octets correspondant au pixel (BGR)
    file_rawRead(position, color, sizeof(uint8_t), 3, file);

    // Attribution des couleurs au pixel (dans l’ordre standard RGB)
    image->pixels[y][x].blue  = color[0]; // 1er octet : bleu
    image->pixels[y][x].green = color[1]; // 2e octet : vert
    image->pixels[y][x].red   = color[2]; // 3e octet : rouge
}


void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    // Cette fonction parcourt chaque pixel de l'image
    // et lit sa valeur RGB à partir du fichier BMP.

    for (int y = 0; y < image->height; y++) {  // Parcourt chaque ligne (de haut en bas)
        for (int x = 0; x < image->width; x++) {  // Parcourt chaque colonne de gauche à droite
            // Lit la couleur du pixel situé à la position (x, y)
            // et stocke les valeurs RGB dans la structure image->pixels
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}


void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    // On calcule la position exacte dans le fichier BMP où se trouve le pixel (x, y).
    // On part du bas de l'image (BMP stocke les lignes à l'envers),
    // donc on inverse y avec (hauteur - 1 - y), puis on avance de x colonnes.
    uint32_t position = BITMAP_OFFSET + ((image->height - 1 - y) * image->width + x) * 3;

    // On prépare les 3 composantes de couleur dans l'ordre : bleu, vert, rouge.
    uint8_t color[3] = {
        image->pixels[y][x].blue,
        image->pixels[y][x].green,
        image->pixels[y][x].red
    };

    // Ensuite, on écrit ces 3 octets dans le fichier à la bonne position.
    // Cela correspond à une couleur de pixel en format BMP 24 bits.
    file_rawWrite(position, color, sizeof(uint8_t), 3, file);
}


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    // Vérification basique : s'assurer que l'image et le fichier existent
    if (!image || !file) {
        printf("Erreur : image ou fichier invalide\n");
        return;
    }

    // Parcours de chaque ligne de l'image
    for (int y = 0; y < image->height; y++) {
        // Parcours de chaque pixel de la ligne
        for (int x = 0; x < image->width; x++) {
            // On écrit la couleur du pixel (x, y) dans le fichier BMP
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}




// Cette fonction applique un filtre négatif à une image 24 bits.
// Elle inverse chaque composante de couleur (rouge, vert, bleu) pixel par pixel.
void bmp24_negative(t_bmp24 *img) {
    // Vérifie que l'image est bien chargée
    if (!img || !img->pixels) {
        printf("Erreur : image invalide\n");
        return;
    }

    // Parcours de chaque ligne de l'image
    for (int y = 0; y < img->height; y++) {
        // Parcours de chaque pixel de la ligne
        for (int x = 0; x < img->width; x++) {
            // Inverse chaque composante de couleur (255 - valeur)
            img->pixels[y][x].red   = 255 - img->pixels[y][x].red;
            img->pixels[y][x].green = 255 - img->pixels[y][x].green;
            img->pixels[y][x].blue  = 255 - img->pixels[y][x].blue;
        }
    }
}


// Cette fonction convertit une image 24 bits en niveaux de gris.
// Elle calcule la moyenne des composantes R, G et B pour chaque pixel,
// puis remplace chaque composante par cette moyenne.
void bmp24_grayscale(t_bmp24 *img) {
    // Vérifie que l'image est valide
    if (!img || !img->pixels) {
        printf("Erreur : image invalide ou non chargée\n");
        return;
    }

    // Parcourt chaque ligne de l'image
    for (int y = 0; y < img->height; y++) {
        // Parcourt chaque pixel de la ligne
        for (int x = 0; x < img->width; x++) {
            // Récupère les valeurs des couleurs d'origine
            uint8_t r = img->pixels[y][x].red;
            uint8_t g = img->pixels[y][x].green;
            uint8_t b = img->pixels[y][x].blue;

            // Calcule une intensité moyenne (simple moyenne arithmétique)
            uint8_t gris = (r + g + b) / 3;

            // Applique cette intensité à chaque composante pour un ton de gris
            img->pixels[y][x].red   = gris;
            img->pixels[y][x].green = gris;
            img->pixels[y][x].blue  = gris;
        }
    }
}


void bmp24_brightness(t_bmp24 *img, int value) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = img->pixels[y][x].red + value;
            int g = img->pixels[y][x].green + value;
            int b = img->pixels[y][x].blue + value;

            img->pixels[y][x].red = (r > 255) ? 255 : (r < 0 ? 0 : r);
            img->pixels[y][x].green = (g > 255) ? 255 : (g < 0 ? 0 : g);
            img->pixels[y][x].blue = (b > 255) ? 255 : (b < 0 ? 0 : b);
        }
    }
}

t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float sumR = 0, sumG = 0, sumB = 0;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            int px = x + kx;
            int py = y + ky;

            if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                t_pixel p = img->pixels[py][px];
                float coeff = kernel[ky + offset][kx + offset];
                sumR += p.red * coeff;
                sumG += p.green * coeff;
                sumB += p.blue * coeff;
            }
        }
    }

    t_pixel result;
    result.red = (sumR > 255) ? 255 : (sumR < 0 ? 0 : (uint8_t)sumR);
    result.green = (sumG > 255) ? 255 : (sumG < 0 ? 0 : (uint8_t)sumG);
    result.blue = (sumB > 255) ? 255 : (sumB < 0 ? 0 : (uint8_t)sumB);

    return result;
}

// Calcule l'histogramme d'une image 8 bits en niveaux de gris.
// Retourne un tableau de 256 cases où chaque case correspond à la fréquence d’un niveau de gris (0 à 255).
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    // Vérifie que l'image et ses données sont valides
    if (!img || !img->data) {
        printf("Erreur : image ou données de l'image NULL\n");
        return NULL;
    }

    // Alloue dynamiquement un tableau de 256 entiers, initialisés à 0
    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) {
        printf("Erreur : échec de l'allocation mémoire pour l'histogramme\n");
        return NULL;
    }

    // Parcours de tous les pixels pour remplir l'histogramme
    for (unsigned int i = 0; i < img->dataSize; i++) {
        unsigned char niveau = img->data[i];  // valeur du pixel (niveau de gris)
        hist[niveau]++;                       // incrémente le compteur pour ce niveau
    }

    return hist;  // retourne le tableau rempli
}







// Calcule la fonction de répartition cumulative (CDF) normalisée
// à partir de l'histogramme donné en entrée.
// Retourne un tableau de 256 entiers représentant la CDF égalisée.
unsigned int *bmp8_computeCDF(unsigned int *hist) {
    // Vérifie que l'histogramme existe
    if (!hist) {
        printf("Erreur : histogramme NULL\n");
        return NULL;
    }

    // Allocation du tableau pour la CDF
    unsigned int *cdf = malloc(256 * sizeof(unsigned int));
    if (!cdf) {
        printf("Erreur d'allocation mémoire pour la CDF\n");
        return NULL;
    }

    // Calcul du nombre total de pixels dans l’image
    unsigned int total = 0;
    for (int i = 0; i < 256; i++) {
        total += hist[i];
    }

    // Recherche du premier niveau de gris non nul (cdf_min)
    unsigned int cdf_min = 0;
    unsigned int somme = 0;
    int trouve = 0;

    // Construction de la CDF normalisée (valeurs entre 0 et 255)
    for (int i = 0; i < 256; i++) {
        somme += hist[i];

        // Mémorise le premier niveau de gris non nul une seule fois
        if (!trouve && somme > 0) {
            cdf_min = somme;
            trouve = 1;
        }

        // Application de la formule d’égalisation de l’histogramme
        if (total == cdf_min) {
            cdf[i] = 0;
        } else {
            cdf[i] = round(((float)(somme - cdf_min) / (total - cdf_min)) * 255);
        }
    }

    return cdf;
}






// Applique une égalisation d'histogramme à une image 8 bits.
// Chaque pixel est remplacé par une valeur issue de l'histogramme égalisé.
void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    // Vérifie que tous les pointeurs nécessaires sont valides
    if (!img || !img->data || !hist_eq) {
        printf("Erreur : image, données de l’image ou histogramme non valides.\n");
        return;
    }

    // Parcours de tous les pixels de l'image
    for (unsigned int i = 0; i < img->dataSize; i++) {
        // Remplace la valeur du pixel par la valeur correspondante dans la table d'égalisation
        unsigned char ancienne_valeur = img->data[i];
        unsigned char nouvelle_valeur = (unsigned char)hist_eq[ancienne_valeur];
        img->data[i] = nouvelle_valeur;
    }
}




