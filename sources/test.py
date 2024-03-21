import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers.convolutional import Conv2D, MaxPooling2D
from keras.optimizers import Adam
from keras.utils.np_utils import to_categorical
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
from keras.preprocessing.image import ImageDataGenerator
import seaborn as sn

# Define your path where images are stored
path = "/media/sf_Echange/test/train"

# Initialize variables
count = 0
images = []
labels = []

# Get list of classes
classes_list = os.listdir(path)
print("Total Classes:", len(classes_list))
noOfClasses = len(classes_list)
print("Importing Classes.....")

# Iterate through each class
for class_name in classes_list:
    img_list = os.listdir(os.path.join(path, class_name))
    
    # Iterate through images in each class
    for img_name in img_list:
        # Read image
        img = cv2.imread(os.path.join(path, class_name, img_name))
        # Convert image to grayscale
        img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        # Equalize histogram
        img_eq = cv2.equalizeHist(img_gray)
        # Resize image to (32x32)
        img_resized = cv2.resize(img_eq, (32, 32))
        # Normalize image
        img_normalized = img_resized.astype('float32') / 255
        # Append image and corresponding label to lists
        images.append(img_normalized)
        labels.append(count)
    
    # Print current class count
    print(count, end=" ")
    count += 1

print("")

# Convert lists to numpy arrays and reshape image data
images = np.array(images)
images = images.reshape(-1, 32, 32, 1)  # Reshape to include single channel for grayscale
labels = np.array(labels)

# Split data into training, testing, and validation sets
X_train, X_temp, y_train, y_temp = train_test_split(images, labels, test_size=0.4, random_state=42)
X_test, X_val, y_test, y_val = train_test_split(X_temp, y_temp, test_size=0.5, random_state=42)

# Convert labels to binary class matrix
y_train = to_categorical(y_train, noOfClasses)
y_test = to_categorical(y_test, noOfClasses)
y_val = to_categorical(y_val, noOfClasses)

# Define data augmentation parameters
datagen = ImageDataGenerator(
    width_shift_range=0.1,
    height_shift_range=0.1,
    zoom_range=0.2,
    rotation_range=10
)

# Define CNN model
model = Sequential([
    Conv2D(60, (5, 5), input_shape=(32, 32, 1), activation='relu'),
    MaxPooling2D(pool_size=(2, 2)),
    Conv2D(60, (5, 5), activation='relu'),
    MaxPooling2D(pool_size=(2, 2)),
    Conv2D(30, (3, 3), activation='relu'),
    MaxPooling2D(pool_size=(2, 2)),
    Conv2D(30, (3, 3), activation='relu'),
    MaxPooling2D(pool_size=(2, 2)),
    Flatten(),
    Dense(500, activation='relu'),
    Dropout(0.5),
    Dense(noOfClasses, activation='softmax')
])

# Compile model
model.compile(Adam(lr=0.001), loss='categorical_crossentropy', metrics=['accuracy'])

# Train model with data augmentation
history = model.fit(
    datagen.flow(X_train, y_train, batch_size=20),
    steps_per_epoch=len(X_train) / 20,
    epochs=10,
    validation_data=(X_val, y_val),
    verbose=1
)

# Evaluate model
score = model.evaluate(X_test, y_test, verbose=0)
print('Test loss:', score[0])
print('Test accuracy:', score[1])

# Plot training history
plt.plot(history.history['accuracy'])
plt.plot(history.history['val_accuracy'])
plt.title('Model Accuracy')
plt.xlabel('Epoch')
plt.ylabel('Accuracy')
plt.legend(['Train', 'Validation'], loc='upper left')
plt.show()

# Plot confusion matrix
y_pred = model.predict(X_test)
y_pred_classes = np.argmax(y_pred, axis=1)
conf_matrix = confusion_matrix(y_test.argmax(axis=1), y_pred_classes)
plt.figure(figsize=(10, 7))
sn.heatmap(conf_matrix, annot=True, fmt='d')
plt.xlabel('Predicted')
plt.ylabel('True')
plt.title('Confusion Matrix')
plt.show()
