#include "monitor.hpp"

void function(string msg) {
    cout << "\nThe message is: " << msg;
}

Monitor monitor = Monitor();
void sigintHandler(int);

int main() {

    monitor.Inicializar(true);

    signal(SIGINT, sigintHandler); // For handling the ^C signal, this way we close the shared memory

    while(true){
        cout << "---" << endl;
        sleep(2);
        if(monitor.writeInMemory("Servidor ejecutando") == 0)
            cout << "\nEscribio";
        else
            cout << "\nNo escribio";
    }
    
    
    return 0; 
}

// Just the server can close the shared memory
void sigintHandler(int signum) {
    exit(0);
}