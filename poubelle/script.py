# Ce script récupère l'image de la camera et isole du bleu foncé afin de détecter 
# les lignes de route puis prend le tier du milieu de l'image, la convertit en noir et blanc
# compte les pixels non noir en bas de l'image et nous renvoie la position moyenne
# de chaque ligne de l'image de base à l'ordonnée un tiers
# Pour quitter le script appuyer sur "q" avec le curseur sur une image ou ctrl+c

import cv2
import numpy as np
import time

# Initialiser la capture vidéo (0 pour la caméra par défaut)
cap = cv2.VideoCapture(0)

# Créer des fenêtres pour l'image d'origine, le résultat et l'image intermédiaire
cv2.namedWindow('Image d\'origine', cv2.WINDOW_NORMAL)
cv2.namedWindow('Masque applique', cv2.WINDOW_NORMAL)
cv2.namedWindow('Image BW', cv2.WINDOW_NORMAL)

# Limiter la fréquence de capture à 10 images par seconde
frame_rate = 10
prev_time = time.time()

while True:
    # Capture du temps écoulé
    current_time = time.time()
    elapsed_time = current_time - prev_time

    if elapsed_time < 1.0 / frame_rate:
        continue  # Attendre pour maintenir le rythme

    # Lire le cadre vidéo de la caméra
    ret, image = cap.read()

    if not ret:
        break

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
    third_height = height // 3  # Taille du tiers

    # Obtenir la région de l'image correspondant au tiers du milieu de l'image
    bottom_third = result[third_height:2 * third_height, :]

    # Convertir en niveaux de gris
    bottom_third_gray = cv2.cvtColor(bottom_third, cv2.COLOR_BGR2GRAY)
    
    #---Image en 50 nuances de gris, on détecte le non noir et calcule les moyennes gauche et droite---

    # Trouver les coordonnées x et y de la ligne bleue en bas du tier
    line_x = cv2.findNonZero(bottom_third_gray[(third_height - 1):, :])

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
        
    #--------------------------Affichages et sortie--------------------------

    # Afficher le résultat et les images intermédiaires
    cv2.imshow('Image d\'origine', image)
    cv2.imshow('Masque applique', result)
    cv2.imshow('Image BW', bottom_third_gray)

    # Attendre que l'utilisateur appuie sur la touche 'q' pour quitter
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    prev_time = current_time

# Libérer la capture vidéo et fermer les fenêtres à la fin
cap.release()
cv2.destroyAllWindows()
