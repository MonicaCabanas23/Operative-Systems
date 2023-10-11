#include "monitor.hpp"

void function(string msg) {
    cout << "\nThe message is: " << msg;
}

Monitor monitor = Monitor();
void sigintHandler(int);

int main() {
    signal(SIGINT, sigintHandler); // For handling the ^C signal, this way we close the shared memory

    if(monitor.Inicializar(false) == 0) {
        while(true) {
            cout << "---" << endl;
            sleep(1);
            if(monitor.readMemory(function) == 0)
                cout << "\nLectura exitosa";
            else
                cout << "\nLectura fallo";
        }   
    } else {
        cout << "\nWe need to run the server";
        // inicializar el servidor
    }

    return 0; 
}

// Just the server can close the shared memory
void sigintHandler(int signum) {
    exit(0);
}