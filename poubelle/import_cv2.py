import cv2.cv as cv
import numpy as np

# Charger l'image depuis un fichier
image = cv2.imread('C:/Echange/marquage.jpg')

# Convertir l'image en espace couleur HSV (Hue, Saturation, Value)
hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# Définir la plage de couleurs bleues dans l'espace HSV
lower_blue = np.array([90, 50, 50])  # Borne inférieure  Couleur, Exigence, Luminosité
upper_blue = np.array([180, 255, 255])  # Borne supérieure

# Créer un masque pour les pixels bleus dans la plage spécifiée
mask = cv2.inRange(hsv, lower_blue, upper_blue)

# Appliquer le masque à l'image d'origine pour extraire les pixels bleus
result = cv2.bitwise_and(image, image, mask=mask)

#-------------Masque appliqué, on découpe l'image en 3-------------

# Découper l'image en tiers horizontaux
height, width, _ = image.shape
third_height = height // 3  # Taille du tiers en bas de l'image

# Obtenir la région de l'image correspondant au tiers en bas de l'image
bottom_third = result[2 * third_height:, :]

# Convertir en niveaux de gris
bottom_third_gray = cv2.cvtColor(bottom_third, cv2.COLOR_BGR2GRAY)

#-------------Image en 50 nuances de gris, on détecte le non noir et calcule les moyennes gauche et droite-------------

# Trouver les coordonnées x et y de la ligne bleue dans le tiers en bas
line_x = cv2.findNonZero(bottom_third_gray[1:,:])
#line_x = cv2.findNonZero(bottom_third_gray)

if line_x is not None:
    # Séparer les coordonnées x en deux groupes autour de votre valeur y
    y = 2 * third_height
    x_values = [pt[0][0] for pt in line_x]
    left_x_values = [x for x in x_values if x < width // 2]
    right_x_values = [x for x in x_values if x >= width // 2]

    # Calculer les moyennes des valeurs x pour les deux groupes
    left_x_average = np.mean(left_x_values)
    right_x_average = np.mean(right_x_values)

    # Afficher les moyennes des coordonnées x
    print(f"Moyenne des lignes bleues à gauche : x = {left_x_average}, y = {y}")
    print(f"Moyenne des lignes bleues à droite : x = {right_x_average}, y = {y}")
    
    
#--------------------------Affichages--------------------------

cv2.namedWindow('Image intermediaire', cv2.WINDOW_NORMAL)
cv2.imshow('Image intermediaire', bottom_third_gray)

# Afficher l'image d'origine et le résultat (facultatif)
cv2.namedWindow('Image d\'origine', cv2.WINDOW_NORMAL)
cv2.imshow('Image d\'origine', image)
cv2.namedWindow('Résultat', cv2.WINDOW_NORMAL)
cv2.imshow('Résultat', result)

# Attendre que l'utilisateur appuie sur une touche
cv2.waitKey(0)

# Fermer les fenêtres
cv2.destroyAllWindows()