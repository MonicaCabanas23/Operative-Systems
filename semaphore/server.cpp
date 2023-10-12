#include <string>
#include <csignal>
#include <fstream>
#include "monitor.hpp"

using namespace std;

// Global variables
int counter;
ofstream outFile("messages.txt", ios::app); // Open the file in append mode
Monitor monitor = Monitor();
bool sent = false;
bool received = false;

void checkExistence();
void updateFile(string);
void reviewer();
int readHandler();
void writeHandler();
void sigIntHandler(int);


int main() {
    checkExistence();
    counter = 0;

    signal(SIGINT, sigIntHandler); // For handling the ^C signal, this way we close the shared memory

    cout << "\nCTRL + C for ending the server. ";

    if(monitor.Inicializar(true) == 0) {
        while (true){
            sleep(2);
            if(readHandler() == 0){
                writeHandler();
            }
        }
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
        cout << "Error executing command." << endl;
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

void updateFile(string text) {
    int pos = text.find(" ");
    string str = text.substr(pos + 1);

    if (!outFile.is_open()) {
        cerr << "Error opening the file." << endl;
        exit(1);
    }

    if(str.compare("messages registered") == 0)
        return;

    // Append text to the file
    outFile << text << endl;
    counter++;
    received = true;
    cout << "\nMessage received: " << text;
}

void reviewer() {
    sent = true;
}

int readHandler() {
    if(monitor.readMemory(updateFile) != 0) 
        return -1;

    return 0;
}

void writeHandler() {
    int attempts = 100;
    string msg = to_string(counter) + " messages registered";

    while(attempts > 0){
        monitor.writeInMemory(msg, reviewer);
        attempts--;

        if(sent)
            break;
    }
    sent = false;
}

// Just the server can close the shared memory
void sigIntHandler(int signum) {
    exit(0); // Terminate the program
}