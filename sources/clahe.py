import cv2

# Lecture de l'image depuis le répertoire actuel
image = cv2.imread('/media/sf_Echange/projet_interfili-re/sources/vitesse81.jpg')

# Redimensionnement de l'image pour la compatibilité
image = cv2.resize(image, (500, 600))

# Conversion de l'image en niveaux de gris
image_bw = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Déclaration de CLAHE avec un clipLimit de 5
clahe = cv2.createCLAHE(clipLimit=5)

# Application de l'histogramme égalisé adaptatif avec CLAHE
final_img = clahe.apply(image_bw)

# Seuillage ordinaire sur la même image
_, ordinary_img = cv2.threshold(image_bw, 155, 255, cv2.THRESH_BINARY)

# Affichage des deux images
cv2.imshow("Seuillage ordinaire", ordinary_img)
cv2.imshow("Image avec CLAHE", final_img)

# Enregistrement de l'image
cv2.imwrite("nvpanneau.jpg", ordinary_img)

# Attente de l'appui sur une touche pour fermer les fenêtres
cv2.waitKey(0)
cv2.destroyAllWindows()
