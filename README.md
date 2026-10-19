# Traitement d'images BMP en C

Projet de cours réalisé par **Sébastien Ramirez et Scott Ngoumela**.

Le programme ouvre une image BMP et applique des filtres depuis un menu en console. Le traitement repose sur les valeurs des pixels, sans bibliothèque de traitement d'images.

## Fonctionnalités

- Images 8 bits en niveaux de gris : négatif, luminosité, seuillage et égalisation d'histogramme.
- Images 24 bits : négatif, luminosité et conversion en niveaux de gris.
- Fonctions de convolution présentes dans la bibliothèque, sans option dédiée dans le menu.

## Compiler

Avec un compilateur C99 et CMake 3.16 ou plus récent :

```sh
cmake -S . -B build
cmake --build build
```

Depuis la racine du projet :

```sh
./build/clion_projets
```

Sous Windows : `.\build\clion_projets.exe`. Avec Visual Studio, chercher l'exécutable dans `build/Debug/`.
Dans CLion, ouvrir le dossier et choisir la racine du projet comme répertoire de travail.

## Exemple

1. Choisir `1` pour les filtres 8 bits.
2. Saisir `lena_gray.bmp`, sans guillemets.
3. Choisir `1` pour appliquer le négatif.
4. Ouvrir `result_8_negative.bmp` avec un visualiseur d'images.

`barbara_gray.bmp` est un autre exemple 8 bits. Utiliser `flowers_color.bmp` avec le menu 24 bits.
Les résultats sont enregistrés dans le répertoire courant. Un filtre remplace son ancien fichier de résultat. Les filtres successifs s'appliquent à l'image déjà modifiée : recharger l'original pour repartir de zéro.

## Organisation du code

- `main.c` : menus et saisies.
- `bmp8.h` : structures et prototypes du projet.
- `bmp8.c` : chargement, sauvegarde, filtres, convolution et histogramme.

Les structures d'origine sont conservées : tableau d'intensités pour le gris, tableau de lignes de pixels pour la couleur. Le négatif calcule `255 - valeur`. La luminosité ajoute une valeur puis borne le résultat entre 0 et 255. Le seuillage donne du blanc au-dessus du seuil et du noir sinon. Le gris couleur utilise la moyenne RGB.

## Format et limites

BMP Windows non compressés, en 8 bits avec palette grise ou en 24 bits. Les en-têtes DIB de 40, 52, 56, 108 et 124 octets sont reconnus. Les lignes avec remplissage et les hauteurs négatives sont prises en compte. La limite de chargement est de 256 Mio de pixels.

La sauvegarde reconstruit un BMP standard : elle ne conserve pas les profils colorimétriques ni les autres métadonnées. Une erreur d'écriture interrompt le programme et peut laisser un résultat incomplet.
Il n'y a pas de prévisualisation dans la console. Les fichiers `.idea/` et `cmake-build-debug/` sont des réglages et sorties de l'ancien environnement, pas les sources à modifier.

Les corrections portent sur les entrées, les fichiers BMP et les bornes de convolution ; les filtres et le fonctionnement du projet sont conservés.
