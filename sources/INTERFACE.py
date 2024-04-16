import cv2
import rospy
import random
from std_msgs.msg import Float32, String

# Initialiser la capture vidéo pour la première caméra détectée
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Erreur : Impossible d'ouvrir le flux vidéo.")
    exit()

# Variables pour stocker les données de vitesse, batterie et mode
speed = 0
battery_level = 0
mode_value = "autonome"

# Callbacks pour mettre à jour les données
def speed_callback(data):
    global speed
    speed = data.data

def battery_callback(data):
    global battery_level
    battery_level = data.data

def mode_callback(data):
    global mode_value
    mode_value = data.data
    
battery_level = 0
increasing = True  # État pour savoir si on augmente ou diminue la valeur de la batterie

def update_battery_level():   #simple touche esthétique pour visualiser la batterie
    global battery_level, increasing
    # Définir le pas d'augmentation ou de diminution
    step = 1  # Vous pouvez ajuster ce pas selon les besoins

    if increasing:
        battery_level += step
        if battery_level >= 100:  # Si atteint 100, commencer à diminuer
            increasing = False
            battery_level = 100
    else:
        battery_level -= step
        if battery_level <= 0:  # Si atteint 0, commencer à augmenter
            increasing = True
            battery_level = 0

# Initialisation de ROS
rospy.init_node('interface_listener', anonymous=True)
rospy.Subscriber("vitesse", Float32, speed_callback)
rospy.Subscriber("batterie", Float32, battery_callback)
rospy.Subscriber("mode", String, mode_callback)

cv2.namedWindow('Interface')

try:
	while not rospy.is_shutdown():
		# Capture frame par frame
		ret, frame = cap.read()
		
		# Si frame est lu correctement ret est True
		if not ret:
			print("Erreur : Impossible de lire le flux vidéo.")
			break
		
		# Simuler la récupération des valeurs de batterie, vitesse et mode
		update_battery_level()
		speed = random.randint(0, 100)
		mode_value = random.randint(0, 1)  # Générer aléatoirement 0 ou 1 pour le mode

		# Définir les dimensions et la position de la barre de batterie
		bar_width_max = 120  # Longueur maximale de la barre (100%)
		bar_height = 12  # Hauteur de la barre
		bar_start_position = (450, 40)  # Position de départ de la barre
		bar_end_position = (bar_start_position[0] + int(bar_width_max * (battery_level / 100)), bar_start_position[1] + bar_height)  # Calculer la fin de la barre
        
		# Dessiner la barre de batterie
		if battery_level < 40:   # Affichage de la barre en rouge si le pourcentage est bas		
			cv2.rectangle(frame, bar_start_position, bar_end_position, (50, 50, 255), -1)
                  
			# Charger l'image alert.png
			alert_img = cv2.imread('alert.png')

			# Redimensionner l'image pour qu'elle s'adapte à la fenêtre
			alert_img = cv2.resize(alert_img, (17, 17))

			# Définir la position de l'image alert.png en haut à droite de la fenêtre
			alert_position = (bar_end_position[0] + 40, 38)

			# Superposer l'image alert.png sur le frame
			frame[alert_position[1]:alert_position[1]+alert_img.shape[0], alert_position[0]:alert_position[0]+alert_img.shape[1]] = alert_img

		elif battery_level > 80:   # Affichage de la barre en vert si le pourcentage est haut
			cv2.rectangle(frame, bar_start_position, bar_end_position, (144, 238, 144), -1)
		else:
			cv2.rectangle(frame, bar_start_position, bar_end_position, (255, 255, 255), -1)
		
		# Afficher le pourcentage à côté de la barre
		battery_text = f"{battery_level:.0f}%"
		cv2.putText(frame, battery_text, (bar_end_position[0]+5, bar_start_position[1]+11), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1)
		
		# Préparer le texte à afficher pour vitesse et mode
		speed_text = f"Vitesse: {speed} km/h"
		mode_text = "manuel" if mode_value == 1 else "autonome"
		
		# Définir la position et le fond du texte pour vitesse
		text_position = (450, 10)  # Position ajustée pour être en haut à gauche
		cv2.putText(frame, speed_text, (text_position[0], text_position[1] + 25), cv2.FONT_HERSHEY_DUPLEX, 0.4, (0, 0, 0), 1)
		
		# Définir la position pour l'affichage du mode en dessous de la vitesse
		mode_text_position = (450, 73)  # Position verticale en dessous de la première ligne
		cv2.putText(frame, "Mode "+mode_text, mode_text_position, cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)		

		# Afficher le frame actuel avec les valeurs de batterie, vitesse et mode
		cv2.imshow('Interface', frame)
		
		# Quitter avec la touche 'q' ou si la fenêtre a été fermée
		if cv2.waitKey(1) == ord('q') or cv2.getWindowProperty('Interface', cv2.WND_PROP_VISIBLE) < 1:
          		break

finally:
    # Quand tout est fini, relâcher la capture
    cap.release()
    cv2.destroyAllWindows()
