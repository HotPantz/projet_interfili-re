import cv2
import numpy as np

# Liste des noms de fichiers d'images
image_files = ['/media/sf_Echange/projet_interfili-re/images/panneau50.jpg']

# Fonction pour vérifier si un polygone est rouge dans l'espace HSV
def is_red_polygon_hsv(image, polygon):
    # Convertir l'image en espace HSV
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Extraire les coordonnées du polygone
    polygon_points = polygon.reshape(-1, 2)

    # Isoler la région du polygone dans l'espace HSV
    roi_hsv = hsv[polygon_points[:, 1].min():polygon_points[:, 1].max(),
                  polygon_points[:, 0].min():polygon_points[:, 0].max()]

    # Calculer la moyenne des valeurs HSV dans le polygone
    average_hsv = np.mean(roi_hsv, axis=(0, 1)).astype(np.uint8)

    # Définir les plages de couleur pour le rouge en espace HSV
    lower_red1 = np.array([176, 95, 161], dtype=np.uint8)
    upper_red1 = np.array([186, 255, 255], dtype=np.uint8)
    lower_red2 = np.array([0, 180, 173], dtype=np.uint8)
    upper_red2 = np.array([255, 255, 255], dtype=np.uint8)

    # Créer des masques pour vérifier si la couleur moyenne est rouge
    mask_red1 = cv2.inRange(average_hsv, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(average_hsv, lower_red2, upper_red2)

    return np.any(mask_red1) or np.any(mask_red2)

# Fonction pour traiter l'image
def process_image(image):
    # Convertir l'image en niveaux de gris
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Appliquer un flou gaussien pour réduire le bruit
    blurred = cv2.GaussianBlur(gray, (9, 9), 2)

    # Détecter les cercles dans l'image
    circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=30, param1=50, param2=30, minRadius=50, maxRadius=70)

    # Définir les plages de couleur pour le rouge et le bleu en espace HSV
    lower_red1 = np.array([0, 116, 124])
    upper_red1 = np.array([56, 255, 255])
    lower_red2 = np.array([160, 120, 100])
    upper_red2 = np.array([180, 255, 255])
    lower_blue = np.array([100, 144, 0])
    upper_blue = np.array([121, 255, 255])

    # Convertir l'image en espace HSV
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Initialiser la variable messages en dehors de la boucle
    messages = []

    if circles is not None:
        for circle in circles[0, :]:
            x, y, r = circle

            # Vérifier si les coordonnées sont valides
            if x - r >= 0 and y - r >= 0 and x + r < image.shape[1] and y + r < image.shape[0]:
                # Isoler la région du cercle
                roi = image[int(y - r):int(y + r), int(x - r):int(x + r)]

                # Convertir la région en espace HSV
                hsv_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

                # Appliquer des masques pour détecter le rouge et le bleu dans la région du cercle
                mask_red1 = cv2.inRange(hsv_roi, lower_red1, upper_red1)
                mask_red2 = cv2.inRange(hsv_roi, lower_red2, upper_red2)
                mask_red = cv2.bitwise_or(mask_red1, mask_red2)

                mask_blue = cv2.inRange(hsv_roi, lower_blue, upper_blue)

                # Compter les pixels dans les masques
                red_pixels = cv2.countNonZero(mask_red)
                blue_pixels = cv2.countNonZero(mask_blue)

                # Détecter la couleur dominante du cercle
                if red_pixels > blue_pixels:
                    message = "Cercle rouge detecte."
                    color = (0, 0, 255)  # Rouge
                    # Ajouter des instructions pour le panneau de vitesse rouge
                    cv2.putText(image, "Panneau de vitesse rouge detecte.", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
                    fichier = open("/media/sf_Echange/projet_interfili-re/data/data.txt", "w")
                    fichier.write("Cercle rouge detecte.")
                    fichier.close()
                elif blue_pixels > red_pixels:
                    message = "Cercle bleu detecte."
                    color = (255, 0, 0)  # Bleu
                    # Ajouter des instructions pour le panneau d'obligation de tourner bleu
                    cv2.putText(image, "Panneau d'obligation de tourner bleu detecte.", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
                    fichier = open("/media/sf_Echange/projet_interfili-re/data/data.txt", "w")
                    fichier.write("Cercle bleu detecte.")
                    fichier.close()
                else:
                    message = "Autre couleur."
                    color = (0, 255, 0)
                    fichier = open("/media/sf_Echange/projet_interfili-re/data/data.txt", "w")
                    fichier.write("Autre couleur detecte.")
                    fichier.close()

                # Ajouter le message à la liste des messages
                messages.append(message)

                # Dessiner le cercle sur l'image originale
                cv2.circle(image, (int(x), int(y)), int(r), color, 4)

        # Afficher les messages dans la fenêtre "Message"
        for i, message in enumerate(messages):
            cv2.putText(image, message, (10, 40 + i * 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    
    # Convertir l'image en niveaux de gris pour le deuxième traitement
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Appliquer un flou gaussien pour réduire le bruit et faciliter la détection des contours
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Utiliser la détection de contours Canny
    edges = cv2.Canny(blurred, 50, 150)

    # Trouver les contours dans l'image
    contours, _ = cv2.findContours(edges.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Parcourir les contours pour trouver le panneau stop
    for contour in contours:
        # Calculer la superficie du contour
        area = cv2.contourArea(contour)

        # Ignorer les petits contours (ajustez la valeur selon vos besoins)
        if area < 50:
            continue
            
        # Approximer le contour à un polygone
        epsilon = 0.01 * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)

        # Si le polygone a 8 côtés (un octogone), c'est probablement un panneau stop
        if len(approx) == 8 and is_red_polygon_hsv(image, approx):
            cv2.drawContours(image, [approx], 0, (0, 255, 0), 2)
            cv2.putText(image, 'Panneau STOP', (approx[0][0][0], approx[0][0][1] - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Afficher l'image résultante
    cv2.imshow('Message', image)

# Parcourir chaque image dans la liste
for image_file in image_files:
    # Charger l'image à partir d'un fichier
    image = cv2.imread(image_file)

    # Vérifier si l'image a été chargée correctement
    if image is None:
        print(f"Impossible de charger l'image {image_file}.")
        continue

    # Traiter l'image actuelle
    process_image(image)

    # Attendre une touche pour passer à l'image suivante
    cv2.waitKey(0)

# Fermer toutes les fenêtres
cv2.destroyAllWindows()
