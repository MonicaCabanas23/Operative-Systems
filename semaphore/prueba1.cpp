#include "monitor.hpp"

void function(string msg) {
    cout << "\nThe message is: " << msg;
}

int main() {
    Monitor monitor = Monitor();

    monitor.Inicializar(true);
    monitor.writeInMemory("Servidor ejecutando");
    
    while(true){
        cout << "Hola";
        sleep(5);
    }
    return 0; 
}