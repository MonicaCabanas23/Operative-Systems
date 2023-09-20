#include <iostream>
#include <string>
#include <csignal>
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
int counter;
ofstream outFile("messages.txt", std::ios::app); // Open the file in append mode

void checkExistence();
int openSharedMemory();
void readMessage();
void updateFile(string);
int sendToClient(string);
//void sigReadHandler(int);
void sigReadHandler(int, siginfo_t*, void*);
void sigintHandler(int);

int main() {
    checkExistence();
    shmd = openSharedMemory();
    counter = 0;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // Indicates signal action with signal info
    sa.sa_sigaction = sigReadHandler;
    sigaction(SIGUSR1, &sa, NULL); // For handling the reading signal sent from a client and getting the pid of the client

    //signal(SIGUSR1, sigReadHandler);
    signal(SIGINT, sigintHandler); // For handling the ^C signal, this way we close the shared memory

    cout << "\nCTRL + C for ending the server. ";

    while (true){
        cout << "\nWaiting for signals from clients";
        sleep(5);
    }

    return 0;
}

void checkExistence() {
    const char* processName = "server";
    char buffer[128];
    string command = "pgrep -o " + string(processName);
    pid_t pid;

    // Open a pipe to the shell command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cout << "Error executing command." << std::endl;
    }

    // Read the PID from the command output
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        pid = static_cast<pid_t>(stoi(buffer));
        if(pid != getpid()) {
            cout << "\nProcess already running with PID: " << pid << endl;
            exit(0);
        }
    }

    // Close the pipe
    pclose(pipe);
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
        cerr << "Error opening the file." << endl;
        exit(1);
    }

    // Append text to the file
    outFile << text << endl;

}

void sigReadHandler(int signum, siginfo_t* info, void* context) {

    try {
        cout << "\n----------------------------------------------"; 
        readMessage();
        counter++;
        string msg = "Current messages in file: " + to_string(counter);
        //Write in the shared memory the value of the counter
        if(sendToClient(msg) == 0) {
            // send signal to the client who asked for it.
            if(kill(info->si_pid, SIGUSR2) == 0)
                cout << "\nSignal sent to " << info->si_pid;
            else cout << "\nError: " << strerror(errno);
        } else {
            cout << "\nCould not write in shared memory";
        }
    } catch(...) {
        cout << strerror(errno);
    }
}

int sendToClient(string message) {
    try {
        char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

        if (ptr == MAP_FAILED) {
            cout << "\nError when trying to write in the shared memory" << endl;
            cout << strerror(errno);
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

// Just the server can close the shared memory
void sigintHandler(int signum) {
    cout << "\n----------------------------------------------"; 
    cout << "\nClosing the shared";
    // Close and unlink the shared memory
    close(shmd);
    shm_unlink(SHM_NAME);
    // Close the file
    outFile.close();
    exit(0); // Terminate the program
}