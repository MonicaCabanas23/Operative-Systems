#include <iostream>
#include <string>
#include <csignal>
#include <semaphore.h>
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

using namespace std;

//Semaphore for handling the race problem
#define SEM_NAME "/temp_sem"

#define SHM_NAME "/IBERO_Shm"
#define SHM_PERMISSION 00600
#define SHM_SIZE 500
int shmd;
sem_t* sem;
// Open the file in append mode
ofstream outFile("messages.txt", std::ios::app);

int openSharedMemory();
sem_t* openSemaphore();
void readMessage();
//string writeMessage();
//int sendToClient(string, int);
void updateFile(string);
void sigintHandler(int);

int main() {
    shmd = openSharedMemory();
    //sem = openSemaphore();

    signal(SIGINT, sigintHandler); // For handling the ^C signal, this way we close the shared memory

    cout << "\nCTRL + C for ending the server. ";

    while (true){
        //sem_wait(sem); // Decrements the semaphore to 0 meaning a block to other processes
        cout << "\n----------------------------------------------"; 
        readMessage();
        sleep(5);
        //sem_post(sem); // Increments the semaphore to 1 meaning a release of the semaphore
    }

    return 0;
}

int openSharedMemory() {
    cout << "Creando la Memoria Compartida!" << std::endl;
    int shmd = shm_open(SHM_NAME, O_CREAT|O_RDWR, SHM_PERMISSION);

    if (shmd == -1) {
        std::cout << "Error creando la Memoria Compartida!" << std::endl;
        exit(1);
        return -1;
    }

    try {
        std::cout << "Asignando tamaño a la Memoria Compartida!" << std::endl;
        auto result = ftruncate(shmd, SHM_SIZE);
        if (result == -1) {
            std::cout << "Error al cambiar el tamaño!" << std::endl;
            throw 1;
        }
    }
    catch (...) {
        close(shmd);
        exit(1);
        return -2; // Error assigning size to the memory
    }

    return shmd;
}

sem_t* openSemaphore() {
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, SHM_PERMISSION, 1); //the server starts whatever operation it needs with the semaphore
    if (sem == SEM_FAILED) {
        std::cerr << "Semaphore creation failed" << std::endl;
        exit(1);
    }

    return sem;
}

void readMessage() {
    try {
        struct stat shmobj_st;
        if (fstat(shmd, &shmobj_st) == -1)
        {
            std::cout << "\nError al tomar las propiedades de la Memoria Compartida!";
            throw 1;
        }

        std::cout << "\nLeyendo de la Memoria Compartida!";
        char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "\nError al leer en la Memoria Compartida!";
            throw 1;
        }
        std::cout << "\nContenido de la Memoria Compartida: "<< ptr;  // Esto se lo mostramos al usuario, entonces esto debe de ir en el cliente
        
        // Check if the shared memory contains data (non-empty)
        size_t dataLength = strlen(ptr);
        if (dataLength > 0)
            updateFile(ptr);
    }
    catch (...) {
        close(shmd);
        exit(1);
    }
}

void updateFile(string text) {

    if (!outFile.is_open()) {
        std::cerr << "Error opening the file." << endl;
        exit(1);
    }

    // Append text to the file
    outFile << text << endl;

}

// Just the server can close the shared memory
void sigintHandler(int signum) {
    cout << "\n----------------------------------------------"; 
    cout << "\nClosing the shared memory and semaphore";
    // Close and unlink the shared memory
    close(shmd);
    shm_unlink(SHM_NAME);
    // Close and unlink the semaphore
    sem_close(sem);
    sem_unlink(SEM_NAME);
    // Close the file
    outFile.close();
}