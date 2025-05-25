o

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








