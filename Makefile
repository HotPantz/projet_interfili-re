# Compiler
CC := g++

# Flags de compilation
CFLAGS := -Wextra -std=c++11 `pkg-config --cflags opencv4`

# Inclure les répertoires et les bibliothèques OpenCV
OPENCV_LIBS := `pkg-config --libs opencv4`

# Fichiers sources
SRCS := sources/main.cpp sources/recog.cpp

# Exécutable de sortie
TARGET := recog

# Règle pour construire le programme
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV_LIBS)

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(TARGET)
