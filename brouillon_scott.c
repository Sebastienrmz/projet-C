#include <stdio.h>

#include <stdio.h>void bmp8_free(t_bmp8 *img) {
    // Vérifie si le pointeur image est valide
    if (img == NULL) {
        printf("Erreur : pointeur NULL dans bmp8_free\n");
        return;
    }

// Libère d'abord les données d'image pour ne pas perdre leur adresse
free(img->data);

// Libère ensuite la structure elle-même
free(img);
}

#include <errno.h>


v