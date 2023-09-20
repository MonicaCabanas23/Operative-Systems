#include <iostream>
#include <string>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std; 

//Semaphore for handling the race problem
#define SEM_NAME "/temp_sem"

#define SHM_NAME "/IBERO_Shm"
#define SHM_PERMISSION 00600
#define SHM_SIZE 500

int openSharedMemory();
sem_t* openSemaphore();
void initServer();
string writeMessage();
int sendToServer(string, int);  //Mostrar en pantalla cuantos mensajes han sido almacenados
// void viewMessagesSaved();

int main() {
    int option = 0; 
    int shmd = openSharedMemory();
    //sem_t* sem = openSemaphore();
    string msg = "";

    if(shmd == -1) {
        cout << "\nThe server is not responding. Wait for a while ";
        initServer();
    }

    while(shmd != -1 && option != 3) {
        cout << "\n----------------------------------------------"; 
        cout << "\nChoose your option: \n1. Write a message.\n2. View messages saved.\n3. End. \nYour option: ";
        cin >> option; 

        switch (option)
        {
        case 1:
            msg = writeMessage();
            if(msg != "")
                //sem_wait(sem); // Waits if another process is writing/reading
                sendToServer(msg, shmd);
                //sem_post(sem); // Increments sem to 1 for indicating that another process can write/read
            break;
        case 2:
            //viewMessagesSaved()
            break;
        case 3: 
            cout << "\nGood bye!\n";
            break;
        default:
            cout << "\nInvalid option. Try again.";
            break;
        }
    }

    return 0;
    
}

int openSharedMemory() {
    std::cout << "Abriendo la Memoria Compartida!" << std::endl;
    int shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION); // Since it has no creation mode, if the shmd does not exist, an error will happen

    if (shmd == -1) {
        std::cout << "Error abriendo la Memoria Compartida! "  << strerror(errno) << std::endl;
        return -1; // Return the error value for executing the server. Este error también sucede si el servidor ya está leyendo la memoria compartida.
    }

    return shmd;
}

sem_t* openSemaphore() {
    // Initialize with the semaphore to 0 since this is not the server. The server is supposed to read first.
    sem_t* sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        cout << "\nSemaphore opening failed";
    }
    return sem;
}

void initServer() {
    // Create a child process and execute the server
    auto pid = fork();

    if (pid < 0) {
        cout << "\nProcess creation failed" << "\n";
    }
    else if (pid == 0) {        
        // Execute the new program
        if (execl("./server", "server", "", "", (char *)0 ) == -1) {
            perror("execl");
            exit(1); // Exit the child process on error
        }

        // execl() should never return, but if it does, an error occurred
        perror("execl");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0); // Waiting for the childs execution to terminate to continue

        cout << "\n-----------------------------------------";
        cout << "\nThe child changed its execution as a clone of this program to executing the path. We are in the parent ";
        cout << "\nServer has been initialized\n";
    }
}

string writeMessage() {
    string message = "";
    cout << "\n----------------------------------------------";
    cout << "\nWrite your message: ";
    cin.ignore();
    getline(cin, message);

    return message;
}

int sendToServer(string message, int shmd) {

    try {
        std::cout << "Escribiendo en la Memoria Compartida!" << std::endl;
        char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

        if (ptr == MAP_FAILED) {
            std::cout << "Error al escribir en la Memoria Compartida!" << std::endl;
            throw 1;
        }

        // Copying the message from the *src (message.c_str()) to the *dest (ptr)
        memcpy(ptr, message.c_str(), sizeof(message));  

    }
    catch (...) {
        close(shmd);
        exit(1);
        return -1;
    }

    return 0;
}