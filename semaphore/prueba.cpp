#include "monitor.hpp"

void function(string msg) {
    cout << "\nThe message is: " << msg;
}

int main() {
    Monitor monitor = Monitor();

    monitor.Inicializar(false);
    //monitor.writeInMemory("Hola");
    return 0; 
}