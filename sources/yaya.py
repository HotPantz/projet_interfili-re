import cv2
import numpy as np
import rospy
import time
from std_msgs.msg import Int32
import os

# Initialiser ROS node
rospy.init_node('panneau_detection_node', anonymous=True)

# Créer un éditeur pour publier dans le topic '/vitesse'
vitesse_publisher = rospy.Publisher('/vitesse', Int32, queue_size=10)

# Fonction pour vérifier si un polygone est rouge dans l'espace HSV
def is_red_polygon_hsv(image, polygon):
    # Extraire les coordonnées du polygone
    polygon_points = polygon.reshape(-1, 2)

    # Isoler la région du polygone dans l'image d'origine
    mask = np.zeros(image.shape[:2], dtype=np.uint8)
    cv2.drawContours(mask, [polygon], -1, (255), -1)
    isolated_region = cv2.bitwise_and(image, image, mask=mask)

    # Convertir la région isolée en espace HSV
    hsv_isolated = cv2.cvtColor(isolated_region, cv2.COLOR_BGR2HSV)

    # Définir les plages de couleur pour le rouge en espace HSV
    lower_red1 = np.array([176, 95, 20], dtype=np.uint8)
    upper_red1 = np.array([186, 255, 255], dtype=np.uint8)
    lower_red2 = np.array([0, 100, 20], dtype=np.uint8)
    upper_red2 = np.array([15, 255, 255], dtype=np.uint8)

    # Créer des masques pour vérifier si la couleur moyenne est rouge
    mask_red1 = cv2.inRange(hsv_isolated, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(hsv_isolated, lower_red2, upper_red2)

    # Compter les pixels rouges dans la région isolée
    red_pixel_count = cv2.countNonZero(mask_red1) + cv2.countNonZero(mask_red2)
    total_pixel_count = cv2.countNonZero(mask)

    # Vérifier si la majorité des pixels dans la région isolée sont rouges
    if total_pixel_count > 0:
    # Vérifier si la majorité des pixels dans la région isolée sont rouges
    	if red_pixel_count / total_pixel_count > 0.5:
        		return True
    	else:
        		return False
    else:
    	return False  # Aucun pixel trouvé, la région est considérée comme non rouge
    
    
# Fonction pour isoler un cercle rouge dans une autre image
def isolate_red_circle(image, circle):
    x, y, r = circle

    # Vérifier si les coordonnées sont valides
    if x - r >= 0 and y - r >= 0 and x + r < image.shape[1] and y + r < image.shape[0]:
        # Isoler la région du cercle
        roi = image[int(y - r):int(y + r), int(x - r):int(x + r)]

        # Sauvegarder l'image analysée dans le dossier "signs"
        # Définir le nombre maximum d'images à enregistrer
        max_images = 10

        # Vérifier si le dossier de sauvegarde existe, sinon le créer
        save_folder = '/media/sf_Echange/projet_interfili-re/signs'
        if not os.path.exists(save_folder):
            os.makedirs(save_folder)

        # Vérifier combien d'images sont déjà enregistrées
        existing_images = len(os.listdir(save_folder))

        # Calculer le nombre d'images à écraser
        images_to_delete = existing_images + max_images - 10

        # Supprimer les images à écraser
        for i in range(images_to_delete):
            image_path = os.path.join(save_folder, f'sign{i+1}.jpg')
            if os.path.exists(image_path):
            os.remove(image_path)

        # Enregistrer l'image
        image_path = os.path.join(save_folder, f'sign{existing_images+1}.jpg')
        cv2.imwrite(image_path, roi)
        return roi
    else:
        return None

# Fonction pour traiter l'image
def process_image(image):
    # Convertir l'image en niveaux de gris
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Appliquer un flou gaussien pour réduire le bruit
    blurred = cv2.GaussianBlur(gray, (9, 9), 2)

    # Détecter les cercles dans l'image
    circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1, minDist=30, param1=90, param2=80, minRadius=50, maxRadius=70)

    # Définir les plages de couleur pour le rouge et le bleu en espace HSV
    lower_red1 = np.array([176, 95, 20])
    upper_red1 = np.array([186, 255, 255])
    lower_red2 = np.array([0, 100, 20])
    upper_red2 = np.array([15, 255, 255])
    lower_blue = np.array([105, 100, 20])
    upper_blue = np.array([135, 255, 255])

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
                    cv2.putText(image, "Panneau de vitesse rouge detecte.", (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
                    vitesse_publisher.publish(1)
                    # Isoler le cercle rouge dans une autre image
                    isolated_circle = isolate_red_circle(image, circle)
                    if isolated_circle is not None:
                        cv2.imshow('Isolated Red Circle', isolated_circle)

                    cv2.putText(image, "Panneau de vitesse rouge detecte.", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
                    fichier = open("/media/sf_Echange/projet_interfili-re/data/data.txt", "w")
                    fichier.write("Cercle rouge detecte.")
                    fichier.close()

                elif blue_pixels > red_pixels:
                    message = "Cercle bleu detecte."
                    color = (255, 0, 0)  # Bleu
                    # Ajouter des instructions pour le panneau d'obligation de tourner bleu
                    cv2.putText(image, "Panneau d'obligation de tourner bleu detecte.", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
                else:
                    message = "Autre couleur."
                    color = (0, 255, 0)

                # Ajouter le message à la liste des messages
                messages.append(message)

                # Dessiner le cercle sur l'image originale
                cv2.circle(image, (int(x), int(y)), int(r), color, 4)

        # Afficher les messages dans la fenêtre "Message"
        for i, message in enumerate(messages):
            cv2.putText(image, message, (10, 40             + i * 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

    # Convertir l'image en niveaux de gris pour le deuxième traitement
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Appliquer un flou gaussien pour réduire le bruit et faciliter la détection des contours
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Utiliser la détection de contours Canny
    edges = cv2.Canny(blurred, 50, 150)

    # Trouver les contours dans l'image
    contours, _ = cv2.findContours(edges.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Parcourir les contours pour trouver le panneau stop
    # Convertir l'image en espace de couleur HSV
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Conversion en niveaux de gris
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Détection des contours
    _, thresh = cv2.threshold(gray, 200, 255, cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Parcourir les contours
    for contour in contours:
    # Calculer la superficie du contour
        area = cv2.contourArea(contour)

        # Ignorer les petits contours (ajustez la valeur selon vos besoins)
        if area < 400:
            continue
        # Approximer le contour à un polygone
        epsilon = 0.011 * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)

        # Vérifier si le contour est un octogone
        if len(approx) == 8:
            # Extraire les coordonnées du polygone
            x, y, w, h = cv2.boundingRect(approx)

            # Isoler la région du polygone dans l'espace HSV
            roi_hsv = hsv[y:y+h, x:x+w]

            # Vérifier si le polygone est rouge
            if is_red_polygon_hsv(roi_hsv, approx):
                # Dessiner le polygone sur l'image
                cv2.drawContours(image, [approx], -1, (0, 255, 0), 2)
                cv2.putText(image, 'Red Octagon', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

            # Publier dans le topic '/vitesse'
            vitesse_publisher.publish(0)

    # Afficher l'image résultante
    cv2.imshow('Message', image)

# Capturer la vidéo depuis la caméra
cap = cv2.VideoCapture(0)

# Vérifier si la capture de la vidéo est réussie
if not cap.isOpened():
    print("Erreur lors de l'ouverture de la caméra.")
else:
    # Boucle de lecture des images
    while True:
        # Lire une frame de la vidéo
        ret, frame = cap.read()

        # Vérifier si la frame est lue avec succès
        if ret:
            # Traiter l'image
            #time.sleep(3)
            process_image(frame)

            # Attendre 1 milliseconde et vérifier si une touche est enfoncée
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        else:
            print("Erreur lors de la lecture de la frame.")
            break

# Libérer la capture vidéo et fermer les fenêtres OpenCV
cap.release()
cv2.destroyAllWindows()

