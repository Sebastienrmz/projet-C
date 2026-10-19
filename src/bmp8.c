#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
/* Lecture little-endian sans conversion de pointeur non aligne. */
static uint32_t read32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static unsigned int read16(const unsigned char *p) {
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8);
}

static void write32(unsigned char *p, uint32_t value) {
    for (int i = 0; i < 4; ++i) p[i] = (unsigned char)(value >> (8 * i));
}

static int32_t read_signed32(const unsigned char *p) {
    uint32_t value = read32(p);
    return (int32_t)(value <= INT32_MAX ? (int64_t)value : (int64_t)value - INT64_C(4294967296));
}

static void fill_info(t_bmp24 *img) {
    img->header_info.size = read32(img->header + 14);
    img->header_info.width = read_signed32(img->header + 18);
    img->header_info.height = read_signed32(img->header + 22);
    img->header_info.planes = (uint16_t)read16(img->header + 26);
    img->header_info.bits = (uint16_t)read16(img->header + 28);
    img->header_info.compression = read32(img->header + 30);
    img->header_info.imagesize = read32(img->header + 34);
    img->header_info.xresolution = read_signed32(img->header + 38);
    img->header_info.yresolution = read_signed32(img->header + 42);
    img->header_info.ncolors = read32(img->header + 46);
    img->header_info.importantcolors = read32(img->header + 50);
}

/* Valide les tailles avant toute allocation. Limite : 256 Mio de pixels. */
static int read_header(FILE *file, unsigned char *header, unsigned int depth,
                       unsigned int *width, unsigned int *height) {
    if (fread(header, 1, 54, file) != 54 || read16(header) != 0x4d42 ||
        read16(header + 26) != 1 || read16(header + 28) != depth ||
        read32(header + 30) != 0) return 0;
    uint32_t dib = read32(header + 14);
    if (!(dib == 40 || dib == 52 || dib == 56 || dib == 108 || dib == 124)) return 0;
    *width = read32(header + 18);
    uint32_t rawHeight = read32(header + 22);
    *height = rawHeight & 0x80000000u ? ~rawHeight + 1u : rawHeight;
    if (!*width || *width > INT_MAX || !*height || *height > INT_MAX) return 0;
    uint64_t bytes = (uint64_t)*width * *height * (depth / 8u);
    uint64_t stride = (((uint64_t)*width * (depth / 8u)) + 3u) & ~UINT64_C(3);
    uint32_t offset = read32(header + 10);
    uint64_t end = offset + stride * *height;
    if (bytes > 256u * 1024u * 1024u || bytes > SIZE_MAX ||
        end > LONG_MAX || offset < 14u + dib) return 0;
    if (fseek(file, 0, SEEK_END) != 0) return 0;
    long size = ftell(file);
    return size >= 0 && (uint64_t)size >= end;
}

/* Reconstruit un BMP standard ; les metadonnees ne sont pas recopiees. */
static void make_header(unsigned char *header, unsigned int width,
                        unsigned int height, unsigned int depth) {
    uint32_t offset = depth == 8 ? 1078 : 54;
    uint32_t stride = (width * (depth / 8u) + 3u) & ~3u;
    memset(header, 0, 54);
    header[0] = 'B'; header[1] = 'M';
    write32(header + 2, offset + stride * height);
    write32(header + 10, offset);
    write32(header + 14, 40);
    write32(header + 18, width);
    write32(header + 22, height);
    header[26] = 1;
    header[28] = (unsigned char)depth;
    write32(header + 34, stride * height);
    if (depth == 8) write32(header + 46, 256);
}

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    t_bmp8 *img = NULL;
    unsigned char *row = NULL;
    if (!file) return NULL;
    img = calloc(1, sizeof *img);
    if (!img || !read_header(file, img->header, 8, &img->width, &img->height)) goto fail;
    uint32_t colors = read32(img->header + 46);
    uint32_t dib = read32(img->header + 14);
    if (!colors) colors = 256;
    if (colors > 256 || read32(img->header + 10) < 14u + dib + colors * 4u ||
        fseek(file, (long)(14u + dib), SEEK_SET) != 0 ||
        fread(img->colorTable, 4, colors, file) != colors) goto fail;
    for (uint32_t i = 0; i < colors; ++i)
        if (img->colorTable[4*i] != img->colorTable[4*i+1] ||
            img->colorTable[4*i+1] != img->colorTable[4*i+2]) goto fail;
    img->colorDepth = 8;
    img->dataSize = img->width * img->height; /* Pixels seuls, sans padding. */
    img->data = malloc(img->dataSize);
    size_t stride = ((size_t)img->width + 3u) & ~(size_t)3;
    row = malloc(stride);
    if (!img->data || !row || fseek(file, (long)read32(img->header + 10), SEEK_SET) != 0) goto fail;
    int topDown = (read32(img->header + 22) & 0x80000000u) != 0;
    for (unsigned int y = 0; y < img->height; ++y) {
        if (fread(row, 1, stride, file) != stride) goto fail;
        size_t dest = (size_t)(topDown ? y : img->height - 1u - y) * img->width;
        for (unsigned int x = 0; x < img->width; ++x) {
            if (row[x] >= colors) goto fail;
            img->data[dest+x] = img->colorTable[4u*row[x]];
        }
    }
    make_header(img->header, img->width, img->height, 8);
    for (unsigned int i = 0; i < 256; ++i) {
        img->colorTable[4*i] = img->colorTable[4*i+1] = img->colorTable[4*i+2] = (unsigned char)i;
        img->colorTable[4*i+3] = 0;
    }
    free(row);
    fclose(file);
    return img;
fail:
    free(row);
    bmp8_free(img);
    fclose(file);
    return NULL;
}

void bmp8_saveImage(t_bmp8 *img, const char *filename) {
    if (!img || !img->data) return;
    FILE *file = fopen(filename, "wb");
    if (!file) { perror(filename); exit(EXIT_FAILURE); }
    make_header(img->header, img->width, img->height, 8);
    file_rawWrite(0, img->header, 1, 54, file);
    file_rawWrite(54, img->colorTable, 1, 1024, file);
    size_t stride = ((size_t)img->width + 3u) & ~(size_t)3;
    unsigned char zeros[3] = {0};
    for (unsigned int y = img->height; y > 0; --y) {
        if (fwrite(img->data + (size_t)(y-1u)*img->width, 1, img->width, file) != img->width ||
            fwrite(zeros, 1, stride-img->width, file) != stride-img->width) {
            fclose(file); fprintf(stderr, "Echec de sauvegarde : %s\n", filename); exit(EXIT_FAILURE);
        }
    }
    if (fclose(file) != 0) { perror(filename); exit(EXIT_FAILURE); }
}

void bmp8_free(t_bmp8 * img) {
	if (img == NULL){     //on verifie si img pointevers quelque chose
		
		return;
	}
	free(img->data);  //on free d'abord cela pour pas perdre l'adresse du data
	free(img);
	}//enfin on free le img global

void bmp8_printInfo(t_bmp8 * img) {
	printf("Image Info:\n");    //on va chercher chaque valeur grace au pointeur img qui pointe vers la struct t_bmp8
	printf("   Width: %u \n", img->width);
	printf("   Height: %u \n", img->height);
	printf("   Colors Depth: %u \n", img->colorDepth);
	printf("   Data Size: %u \n", img->dataSize);
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
    if (value < -255) value = -255;
    if (value > 255) value = 255;
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

    if (!kernel || kernelSize <= 0 || kernelSize % 2 == 0 ||
        (unsigned int)kernelSize > img->width || (unsigned int)kernelSize > img->height) return;
    for (int k = 0; k < kernelSize; ++k) {
        if (!kernel[k]) return;
        for (int j = 0; j < kernelSize; ++j) if (!isfinite(kernel[k][j])) return;
    }
    int width = (int)img->width;
    int height = (int)img->height;
    int n = kernelSize / 2;

    //Allouer une copie temporaire pour stocker les nouveaux pixels
    unsigned char *tempData = malloc(img->dataSize);
    if (tempData == NULL) {
        printf("Error: failed to allocate memory for filtered image\n");
        return;
    }

    for (int y = n; y < height - n; y++) {
        for (int x = n; x < width - n; x++) {
            double sum = 0.0;

            for (int i = -n; i <= n; i++) {
                for (int j = -n; j <= n; j++) {
                    int pixel = img->data[(y + i) * width + (x + j)];
                    float coeff = kernel[i + n][j + n];
                    sum += pixel * (double)coeff;
                }
            }

            // Clamp entre 0 et 255
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            tempData[y * width + x] = (unsigned char)sum;
        }
    }

    //Copier le résultat dans l’image d’origine
    for (int y = n; y < height - n; y++) {
        for (int x = n; x < width - n; x++) {
            img->data[y * width + x] = tempData[y * width + x];
        }
    }

    free(tempData);
}

t_pixel ** bmp24_allocateDataPixels(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;
    t_pixel **pixels = malloc((size_t)height * sizeof(t_pixel *));
    if (pixels == NULL) return NULL;

    for (int i = 0; i < height; i++) {
        pixels[i] = calloc((size_t)width, sizeof(t_pixel));
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
    if (width <= 0 || height <= 0 || colorDepth != 24 ||
        (uint64_t)(unsigned int)width * (unsigned int)height * 3u > 256u * 1024u * 1024u)
        return NULL;
    t_bmp24 *img = calloc(1, sizeof(t_bmp24));
    if (img == NULL) return NULL;

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    make_header(img->header, (unsigned int)width, (unsigned int)height, 24);
    fill_info(img);
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
    if (!file) return NULL;
    unsigned char header[54];
    unsigned int width, height;
    if (!read_header(file, header, 24, &width, &height)) { fclose(file); return NULL; }
    t_bmp24 *img = bmp24_allocate((int)width, (int)height, 24);
    if (!img) { fclose(file); return NULL; }
    memcpy(img->header, header, 54);
    fill_info(img);
    bmp24_readPixelData(img, file);
    fclose(file);
    return img;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    if (!img || !img->pixels) return;
    FILE *file = fopen(filename, "wb");
    if (!file) { perror(filename); exit(EXIT_FAILURE); }
    make_header(img->header, (unsigned int)img->width, (unsigned int)img->height, 24);
    fill_info(img);
    file_rawWrite(0, img->header, 1, 54, file);
    bmp24_writePixelData(img, file);
    if (fclose(file) != 0) { perror(filename); exit(EXIT_FAILURE); }
}

void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    // On place le curseur du fichier à la position souhaitée (en octets depuis le début)
    if (fseek(file, (long)position, SEEK_SET) != 0 || fread(buffer, size, n, file) != n) {
        fprintf(stderr, "Erreur de lecture BMP.\n"); exit(EXIT_FAILURE);
    }
}

void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    if (fseek(file, (long)position, SEEK_SET) != 0 || fwrite(buffer, size, n, file) != n) {
        fprintf(stderr, "Erreur d'ecriture BMP.\n"); exit(EXIT_FAILURE);
    }
}

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    // Calcul de la position du pixel dans le fichier BMP.
    // On part de l'offset de l'image (BITMAP_OFFSET), puis on ajoute l'index du pixel dans le tableau.
    // L'image BMP étant stockée à l’envers (de bas en haut), on inverse la ligne : (height - 1 - y).
    uint32_t stride = ((uint32_t)image->width * 3u + 3u) & ~3u;
    uint32_t row = read32(image->header + 22) & 0x80000000u ? (uint32_t)y : (uint32_t)(image->height - 1 - y);
    uint32_t position = read32(image->header + 10) + row * stride + (uint32_t)x * 3u;

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
    uint32_t stride = ((uint32_t)image->width * 3u + 3u) & ~3u;
    uint32_t row = read32(image->header + 22) & 0x80000000u ? (uint32_t)y : (uint32_t)(image->height - 1 - y);
    uint32_t position = read32(image->header + 10) + row * stride + (uint32_t)x * 3u;

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
        uint32_t stride = ((uint32_t)image->width * 3u + 3u) & ~3u;
        unsigned char zeros[3] = {0};
        uint32_t pos = read32(image->header + 10) + (uint32_t)(image->height - 1 - y) * stride + (uint32_t)image->width * 3u;
        file_rawWrite(pos, zeros, 1, stride - (uint32_t)image->width * 3u, file);
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
    if (!img || !img->pixels) return;
    if (value < -255) value = -255;
    if (value > 255) value = 255;
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
    t_pixel black = {0, 0, 0};
    if (!img || !img->pixels || !kernel || kernelSize <= 0 || kernelSize % 2 == 0 ||
        kernelSize > img->width || kernelSize > img->height || x < 0 || y < 0 ||
        x >= img->width || y >= img->height) return black;
    for (int k = 0; k < kernelSize; ++k) {
        if (!kernel[k]) return black;
        for (int j = 0; j < kernelSize; ++j) if (!isfinite(kernel[k][j])) return black;
    }
    int offset = kernelSize / 2;
    double sumR = 0, sumG = 0, sumB = 0;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            int px = x + kx;
            int py = y + ky;

            if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                t_pixel p = img->pixels[py][px];
                float coeff = kernel[ky + offset][kx + offset];
                sumR += p.red * (double)coeff;
                sumG += p.green * (double)coeff;
                sumB += p.blue * (double)coeff;
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
            cdf[i] = (unsigned int)i;
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
