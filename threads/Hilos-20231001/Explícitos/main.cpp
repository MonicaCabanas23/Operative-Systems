#include <iostream>
#include "explicitos.hpp"

using namespace std;


int main() {

    cout << "Opciones:" << endl;
    cout << "    1- Ejemplo 1" << endl;
    cout << "    2- Ejemplo 2" << endl;
    cout << "    3- Ejemplo 3" << endl;
    cout << "    4- Ejemplo 4" << endl;
    cout << "    Opción:";
    unsigned i ;
    cin >> i;
    switch (i) {
        case 1:
            cout << "Hola estoy en el ejemplo 1" << endl; 
            test_threads1();
            break;
        case 2:
            cout << "Hola estoy en el ejemplo 2" << endl;  
            test_threads2();
            break;
        case 3:
            cout << "Hola estoy en el ejemplo 1A" << endl; 
            test_threads1A();
            break;
        case 4:
            cout << "Hola estoy en el asíncrono" << endl; 
            test_async();
            break;
        default:
            cout << "Opción desconocida";
    }
    return 0;
}
