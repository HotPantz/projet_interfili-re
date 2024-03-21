import cv2
import random

# Initialiser la capture vidéo pour la première caméra détectée
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Erreur : Impossible d'ouvrir le flux vidéo.")
    exit()

cv2.namedWindow('Flux vidéo Webcam')

try:
    while True:
        # Capture frame par frame
        ret, frame = cap.read()

        # Si frame est lu correctement ret est True
        if not ret:
            print("Erreur : Impossible de lire le flux vidéo.")
            break

        # Simuler la récupération des valeurs de batterie et vitesse
        battery_level = random.uniform(0, 100)
        speed = random.randint(0, 100)

        # Préparer le texte à afficher pour batterie et vitesse
        text = f"Batterie: {battery_level:.2f}%, Vitesse: {speed} km/h"
        # Définir la position et le fond du texte
        text_position = (frame.shape[1] - 225, 30)
        cv2.rectangle(frame, (text_position[0], text_position[1] - 15), (frame.shape[1], text_position[1] + 2), (255, 255, 255), -1)
        cv2.putText(frame, text, text_position, cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 0), 1)

        # Afficher le frame actuel avec les valeurs de batterie et vitesse
        cv2.imshow('Flux vidéo Webcam', frame)

        # Quitter avec la touche 'q' ou si la fenêtre a été fermée
        if cv2.waitKey(1) == ord('q') or cv2.getWindowProperty('Flux vidéo Webcam', cv2.WND_PROP_VISIBLE) < 1:
            break
finally:
    # Quand tout est fini, relâcher la capture
    cap.release()
    cv2.destroyAllWindows()
