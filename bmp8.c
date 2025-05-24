#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include <stdint.h>

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
            // Libère déjà alloués
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
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Lecture des 54 octets d'en-tête
    file_rawRead(0, img->header, sizeof(uint8_t), 54, file);

    // Lecture du header info
    file_rawRead(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    img->pixels = bmp24_allocateDataPixels(img->width, img->height);
    if (!img->pixels) {
        fclose(file);
        free(img);
        return NULL;
    }

    // Extraire l'offset depuis le tableau header
    uint32_t offset = *(uint32_t *)(img->header + BITMAP_OFFSET);

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
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible de créer le fichier %s\n", filename);
        return;
    }

    // Écriture de l'en-tête
    file_rawWrite(0, img->header, sizeof(uint8_t), 54, file);

    // Écriture des informations supplémentaires
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Récupération de l'offset
    uint32_t offset = *(uint32_t *)(img->header + BITMAP_OFFSET);

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t color[3];
            color[0] = img->pixels[y][x].blue;
            color[1] = img->pixels[y][x].green;
            color[2] = img->pixels[y][x].red;
            uint32_t pos = offset + ((img->height - 1 - y) * img->width + x) * 3;
            file_rawWrite(pos, color, sizeof(uint8_t), 3, file);
        }
    }

    fclose(file);
}










void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}


void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint32_t position = BITMAP_OFFSET + ((image->height - 1 - y) * image->width + x) * 3;
    uint8_t color[3];
    file_rawRead(position, color, sizeof(uint8_t), 3, file);
    image->pixels[y][x].blue = color[0];
    image->pixels[y][x].green = color[1];
    image->pixels[y][x].red = color[2];
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint32_t position = BITMAP_OFFSET + ((image->height - 1 - y) * image->width + x) * 3;
    uint8_t color[3] = {
        image->pixels[y][x].blue,
        image->pixels[y][x].green,
        image->pixels[y][x].red
    };
    file_rawWrite(position, color, sizeof(uint8_t), 3, file);
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}




void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->pixels[y][x].red = 255 - img->pixels[y][x].red;
            img->pixels[y][x].green = 255 - img->pixels[y][x].green;
            img->pixels[y][x].blue = 255 - img->pixels[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t r = img->pixels[y][x].red;
            uint8_t g = img->pixels[y][x].green;
            uint8_t b = img->pixels[y][x].blue;
            uint8_t gray = (r + g + b) / 3;
            img->pixels[y][x].red = gray;
            img->pixels[y][x].green = gray;
            img->pixels[y][x].blue = gray;
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

unsigned int * bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) {
        printf("Erreur : image ou data NULL\n");
        return NULL;
    }

    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) {
        printf("Erreur d'allocation pour histogramme\n");
        return NULL;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        unsigned char pixel = img->data[i];
        hist[pixel]++;
    }

    return hist;
}




#include <math.h>

unsigned int * bmp8_computeCDF(unsigned int *hist) {
    if (!hist) {
        printf("Erreur : histogramme NULL\n");
        return NULL;
    }

    unsigned int *cdf = malloc(256 * sizeof(unsigned int));
    if (!cdf) {
        printf("Erreur d'allocation pour CDF\n");
        return NULL;
    }

    unsigned int total = 0;
    for (int i = 0; i < 256; i++) {
        total += hist[i];
    }

    unsigned int cdf_min = 0;
    unsigned int sum = 0;
    int found = 0;

    for (int i = 0; i < 256; i++) {
        sum += hist[i];
        if (!found && sum > 0) {
            cdf_min = sum;
            found = 1;
        }

        if (total == cdf_min) {
            cdf[i] = 0;
        } else {
            cdf[i] = round(((float)(sum - cdf_min) / (total - cdf_min)) * 255);
        }
    }

    return cdf;
}





void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    if (!img || !img->data || !hist_eq) {
        printf("Erreur : image ou histogramme égalisé NULL\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq[img->data[i]];
    }
}




