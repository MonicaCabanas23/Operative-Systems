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
bool sigReceived = false;
bool acceptUserInput = true;

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

    signal(SIGUSR2, sigReadHandler); // For handling when the server sends signal for reading

    if(shmd == -1) {
        cout << "\nThe server is not responding. Wait for a while ";
        initServer();
    } else server_pid = getServerPID(); // If the server exists, get the PID for sending signals

    while(shmd != -1 && option != 3 && kill(server_pid, 0) == 0) {
        sleep(1);
        // This is for first reading what is in the shared memory, and then asking the user for an entry
        if (sigReceived) {
            acceptUserInput = false; // Pause user input
            readMessage();
            acceptUserInput = true; // Resume user input
            sigReceived = false; // Reset the flag
        } 
        if (acceptUserInput) {
            cout << "\n----------------------------------------------"; 
            cout << "\nChoose your option: \n1. Write text and send it to server.\n2. Request quantity of messages.\n3. End. \nYour option: ";
            cin >> option; 
        }

        switch (option){
            case 1:
                msg = writeMessage();
                if(msg != "")
                if(sendToServer(msg) == 0) {
                    //send signal to server to read the message
                    if(kill(server_pid, SIGUSR1) == 0) {
                        cout << "\n----------------------------------------------";
                        cout << "\nServer notified of the sent message.";
                    }
                    else {
                        cout << "\n----------------------------------------------\n";
                        cout << strerror(errno) << endl;
                    }
                }
            break;
            case 2:
                //send signal to server to return how many text entries are saved
                if(kill(server_pid, SIGUSR2) == 0) {
                    cout << "\n----------------------------------------------";
                    cout << "\nServer notified of the request for quantity of messages.";
                }
                else {
                    cout << "\n----------------------------------------------\n";
                    cout << "\nServer is not running";
                    initServer();
                } 
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
        cout << "\nError executing command." << endl;
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
    cout << "----------------------------------------------";
    cout << "\nOpening shared memory" << endl;
    int shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION); // Since it has no creation mode, if the shmd does not exist, an error will happen

    if (shmd == -1) {
        cout << "\nError trying to open the shared memory: "  << strerror(errno) << endl;
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
            cout << "\nError getting properties of shared memory";
            throw 1;
        }

        char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
        if (ptr == MAP_FAILED) {
            cout << "\nError reading shared memory";
            throw 1;
        }
        cout << "\nResponse from server: "<< ptr;  // Quantity of messages saved
    }
    catch (...) {
        close(shmd);
        exit(1);
    }
}

int sendToServer(string message) {

    try {
        cout << "\nWriting message in shared memory" << endl;
        char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

        if (ptr == MAP_FAILED) {
            cout << "\nError when trying to write in the shared memory" << strerror(errno) << endl;
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
    sigReceived = true;
}