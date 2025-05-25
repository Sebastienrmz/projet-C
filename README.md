Ce projet a été réalisé dans le cadre du module de traitement d'images en langage C.
FONCTIONNALITÉS PRINCIPALES :
------------------------------
Lecture et affichage d'informations sur les images BMP en niveaux de gris (8 bits) et couleur (24
bits)
Application de filtres de base :
- Négatif
- Ajustement de la luminosité
- Seuillage / Binarisation
- Conversion en niveaux de gris (pour les 24 bits)
  Égalisation d'histogramme sur les images 8 bits pour améliorer le contraste
  UTILISATION :
------------------------------
1. Lancer le programme
2. Choisir l'action souhaitée (ouvrir une image, appliquer un filtre, sauvegarder)
3. Saisir le chemin de l'image BMP à traiter (ex: ../images/lenagray.bmp)
4. Choisir le filtre et entrer les paramètres si besoin (valeur de luminosité, seuil, etc.)
5. L'image filtrée est automatiquement enregistrée dans le dossier du projet
   STRUCTURE DU CODE :
------------------------------
- main.c : contient le menu interactif principal
- bmp8.c / bmp8.h : toutes les fonctions de traitement d'images 8 bits et 24 bits sont ici
- Les images sont gérées sous forme de structures contenant les données + en-têtes
  FORMAT DES IMAGES :
------------------------------
Le programme fonctionne uniquement avec des fichiers .bmp non compressés (24 bits couleur ou 8
bits niveaux de gris).

Bugs connus
- Le chargement dimages tres lourdes peut prendre plusieurs secondes (li la lecture manuelle des
  pixels).
- L'affichage en temps reel nest pas intgrer pour visualiser le resultat, il faut ouvrir limage sauvegarde
  avec un visualiseur BMP externe.
- Seules les images BMP non compresses sont prises en charge.
- Pas de verification avance sur la validit des fichiers fournis (lutilisateur doit entrer un chemin
  correct)..

------------------------------
AUTEURS :
- Sébastien RAMIREZ
- Scott NGOUMELA
 
- Ce projet est 100% en C et a été testé sous CLion avec CMake.