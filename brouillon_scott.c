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
