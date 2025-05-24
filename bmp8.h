#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// =================== BMP 8 BITS ===================
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(t_bmp8 * img, const char * filename);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(t_bmp8 * img);
void bmp8_negative(t_bmp8 * img);
void bmp8_brightness(t_bmp8 * img, int value);
void bmp8_threshold(t_bmp8 * img, int threshold);
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);

// =================== BMP 24 BITS ===================
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;

typedef struct {
    uint8_t header[54];
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **pixels;
} t_bmp24;

// Constantes BMP
#define BITMAP_MAGIC      0x00
#define BITMAP_SIZE       0x02
#define BITMAP_OFFSET     0x0A
#define BITMAP_WIDTH      0x12
#define BITMAP_HEIGHT     0x16
#define BITMAP_DEPTH      0x1C
#define BITMAP_SIZE_RAW   0x22
#define BMP_TYPE          0x4D42
#define HEADER_SIZE       0x0E
#define INFO_SIZE         0x28
#define DEFAULT_DEPTH     0x18

// Fonctions BMP 24 bits
t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);

void bmp24_negative(t_bmp24 * img);
void bmp24_grayscale(t_bmp24 * img);
void bmp24_brightness(t_bmp24 * img, int value);

t_pixel bmp24_convolution(t_bmp24 * img, int x, int y, float ** kernel, int kernelSize);

void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

void bmp24_readPixelValue(t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData(t_bmp24 * image, FILE * file);
void bmp24_writePixelValue(t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData(t_bmp24 * image, FILE * file);

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);

unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);


#endif // BMP8_H