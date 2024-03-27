import cv2
import tkinter as tk
from threading import Thread
import rospy
from std_msgs.msg import Bool, Float32
import random

class Application:
    def __init__(self, window, window_title):
        self.window = window
        self.window.title(window_title)

        # ROS setup
        rospy.init_node('interface_node', anonymous=True)
        self.mode_publisher = rospy.Publisher('mode_topic', Bool, queue_size=10)
        self.speed_publisher = rospy.Publisher('speed', Float32, queue_size=10)
        self.battery_publisher = rospy.Publisher('battery', Float32, queue_size=10)

        # Initial mode is Manual (1)
        self.mode = 1

        # Video capture
        self.cap = cv2.VideoCapture(0)
        if not self.cap.isOpened():
            raise Exception("Could not open video device")

        # UI Elements
        self.toggle_button = tk.Button(window, text="Toggle Mode (Currently Manual)", command=self.toggle_mode)
        self.toggle_button.pack()

        self.speed_label = tk.Label(window, text="Speed: 0")
        self.speed_label.pack()

        self.battery_label = tk.Label(window, text="Battery: 100")
        self.battery_label.pack()

        # Start the video loop and ROS data publishing in separate threads
        self.video_thread = Thread(target=self.video_loop)
        self.video_thread.daemon = True
        self.video_thread.start()

        self.ros_thread = Thread(target=self.ros_loop)
        self.ros_thread.daemon = True
        self.ros_thread.start()

        # Close the window and release the capture properly
        self.window.protocol("WM_DELETE_WINDOW", self.on_closing)

    def toggle_mode(self):
        self.mode = 0 if self.mode else 1
        self.toggle_button.config(text=f"Toggle Mode (Currently {'Auto' if self.mode == 0 else 'Manual'})")
        self.mode_publisher.publish(self.mode)

    def video_loop(self):
        while True:
            ret, frame = self.cap.read()
            if ret:
                cv2.imshow("Video Feed", frame)
                
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            else:
                break
        self.cap.release()
        cv2.destroyAllWindows()

    def ros_loop(self):
        rate = rospy.Rate(1)  # 1Hz
        while not rospy.is_shutdown():
            # Generate random speed and battery values
            speed = random.uniform(0, 100)
            battery = random.uniform(0, 100)

            # Publish the values
            self.speed_publisher.publish(speed)
            self.battery_publisher.publish(battery)

            # Update the labels
            self.speed_label.config(text=f"Speed: {speed:.2f}")
            self.battery_label.config(text=f"Battery: {battery:.2f}")

            rate.sleep()

    def on_closing(self):
        self.cap.release()
        cv2.destroyAllWindows()
        self.window.destroy()

# Create a window and pass it to the Application object
root = tk.Tk()
app = Application(root, "Interface")
root.mainloop()
