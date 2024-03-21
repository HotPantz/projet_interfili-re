#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "/media/sf_Echange/projet_interfili-re/headers/recog.hpp"

int main() {
    
    int error;
    pid_t pid;

    printf("debut\n");

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } 

    if(pid == 0){

        const char *args[] = {"python", "/media/sf_Echange/projet_interfili-re/sources/panneau_stop_vitesse.py", nullptr}; // Program name and its arguments
        execvp("python", const_cast<char *const *>(args)); // Execute the Python script
        perror("execvp"); // Print error message if execvp fails
    }

    else if(pid > 0) {

        waitpid(1,NULL,0); // Wait for the child process to finish

        printf("test 1\n");

        // Read data from file
        std::ifstream file("/media/sf_Echange/projet_interfili-re/data/data.txt");
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open data.txt\n";
            return 1;
        }

        printf("test 2\n");

        std::string data;
        std::getline(file, data); // Assuming you want to read the whole line

        // Check if the line contains "Cerlce rouge detecte."
        if (data.find("Cercle rouge detecte.") != std::string::npos) {
            printf("cercle rouge\n");
            error = recog(); // Execute recog() if the line contains "Cerlce rouge detecte."
        }

        file.close(); // Close the file

        printf("fini\n");
    }

    return 0;
}
