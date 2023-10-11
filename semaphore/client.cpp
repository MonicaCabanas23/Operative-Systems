#include <string>
#include <csignal>
#include "monitor.hpp"

using namespace std;

// Monitor creation
Monitor monitor = Monitor();

// Functions prototypes
void displayMenu();
pid_t getServerPID();
void initServer();
string getMessage();
void sigintHandler(int);

void reader(string msg) {
    cout << "\nThe message is: " << msg;
}

int main() {
    pid_t server_pid;

    // If the server is not running the monitor initialization will return -1 code as an error
    if(monitor.Inicializar(false) != 0) 
        initServer();
    else server_pid = getServerPID(); // If the server exists, get the PID for sending signals

    displayMenu();

    return 0;
}

void displayMenu() {
    int option = 0; 

    while(option != 3) {
        cout << "\n----------------------------------------------"; 
        cout << "\nChoose your option: \n1. Write text and send it to server.\n2. Request quantity of messages.\n3. End. \nYour option: ";
        cin >> option; 

        switch (option){
            case 1: {
                string msg = "";
                msg = getMessage();
                // Critical zone
                if(msg != "") 
                    monitor.writeInMemory(msg);
                break;
            }
            case 2:
            // Critical zone
                monitor.readMemory(reader);
                break;
            case 3: 
                cout << "\nGood bye!\n";
                break;
            default:
                cout << "\nInvalid option.";
                break;
        }

    }
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
        waitpid(pid, &status, 0); // Waiting for the child's execution to terminate to continue
    }
}

string getMessage() {
    string message = "";
    cout << "\n----------------------------------------------";
    cout << "\nWrite your message: ";
    cin.ignore();
    getline(cin, message);

    return message;
}