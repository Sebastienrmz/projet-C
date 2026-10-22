@echo off
setlocal
cd /d "%~dp0"
if not exist "filtres.exe" goto absent
:menu
cls
echo FILTRES D'IMAGES
echo.
echo 1 - Tester le negatif sur une image en niveaux de gris
echo 2 - Tester le negatif sur une image couleur
echo 3 - Ouvrir le menu complet du projet
echo 0 - Quitter
choice /c 1230 /n /m "Votre choix : "
if errorlevel 4 exit /b 0
if errorlevel 3 goto complet
if errorlevel 2 goto couleur
(echo 1&echo images/lena_gray.bmp&echo 1&echo 0&echo 0)|filtres.exe
if errorlevel 1 goto erreur
if not exist result_8_negative.bmp goto erreur
start "" "result_8_negative.bmp"
goto fin
:couleur
(echo 2&echo images/flowers_color.bmp&echo 1&echo 0&echo 0)|filtres.exe
if errorlevel 1 goto erreur
if not exist result_24_negative.bmp goto erreur
start "" "result_24_negative.bmp"
goto fin
:complet
echo Images fournies : images/lena_gray.bmp et images/flowers_color.bmp
filtres.exe
if errorlevel 1 goto erreur
:fin
echo.
echo Les images resultats sont enregistrees dans ce dossier.
pause
goto menu
:absent
echo Extrayez tout le ZIP avant de lancer Tester.bat.
pause
exit /b 1
:erreur
echo Le programme a rencontre une erreur. Consultez le message ci-dessus.
pause
exit /b 1
