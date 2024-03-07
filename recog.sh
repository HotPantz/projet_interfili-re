#!/bin/bash

# Compiler le code avec make
make

# Vérifier si la compilation a réussi
if [ $? -eq 0 ]; then
    # Exécuter l'application
    ./recog
else
    echo "Erreur lors de la compilation."
fi
