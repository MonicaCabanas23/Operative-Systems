#include <iostream>
#include <string>
#include <csignal>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

using namespace std; 

#define SHM_NAME "/IBERO_Shm"
#define SHM_PERMISSION 00600
#define SHM_SIZE 500
int shmd;

pid_t getServerPID();
int openSharedMemory();
void initServer();
string writeMessage();
void readMessage();
int sendToServer(string);  // Mostrar en pantalla cuantos mensajes han sido almacenados
void sigReadHandler(int); // Handler for reading the shared memory


int main() {
    shmd = openSharedMemory();
    int option = 0; 
    string msg = "";
    pid_t server_pid;

    //signal(SIGUSR2, sigReadHandler); // For handling when the server sends signal for reading

    if(shmd == -1) {
        cout << "\nThe server is not responding. Wait for a while ";
        initServer();
    } else server_pid = getServerPID(); // If the server exists, get the PID for sending signals

    while(shmd != -1 && option != 3) {
        cout << "\n----------------------------------------------"; 
        cout << "\nChoose your option: \n1. Write a message.\n2. View messages saved.\n3. End. \nYour option: ";
        cin >> option; 

        switch (option)
        {
        case 1:
            msg = writeMessage();
            if(msg != "")
                if(sendToServer(msg) == 0) {
                    //send signal to server to read the message
                    if(kill(server_pid, SIGUSR1) == 0)
                        cout << "\nServer notified for reading the shared memory";
                    else cout << strerror(errno) << endl;
                }
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

pid_t getServerPID() {
    const char* processName = "server";
    char buffer[128];
    string command = "pgrep -o " + string(processName);
    pid_t pid;

    // Open a pipe to the shell command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cout << "Error executing command." << std::endl;
        return 1;
    }

    // Read the PID from the command output
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        pid = static_cast<pid_t>(stoi(buffer));
        cout << "\nPID of " << processName << " is " << pid << endl;
    } else {
        cout << "\nProcess not found ";
    }

    // Close the pipe
    pclose(pipe);

    return pid;
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

void readMessage() {
    try {
        struct stat shmobj_st;
        if (fstat(shmd, &shmobj_st) == -1)
        {
            std::cout << "\nError getting properties of shared memory";
            throw 1;
        }

        char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "\nError reading shared memory";
            throw 1;
        }
        std::cout << "\nMessage sent from server: "<< ptr;  // Quantity of messages saved
    }
    catch (...) {
        close(shmd);
        exit(1);
    }
}

int sendToServer(string message) {

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

void sigReadHandler(int signum) {
    readMessage();
}