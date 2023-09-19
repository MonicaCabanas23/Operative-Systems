#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/IBERO_Shm"
#define SHM_PERMISSION 00600
#define SHM_SIZE 500

void writeMessage();
void sendToServer();  //Mostrar en pantalla cuantos  mensajes han sido almacenados
void readMessagesQuantity();


int main() {
    int num = 0;
        std::cout << "Creando la Memoria Compartida!" << std::endl;
        int shmd = shm_open(SHM_NAME, O_CREAT|O_RDWR, SHM_PERMISSION);
        if (shmd == -1) {
            std::cout << "Error creando la Memoria Compartida!" << std::endl;
            exit(1);
        }
    while (num != 3) {

        try {
            std::cout << "Asignando tamaño a la Memoria Compartida!" << std::endl;
            auto result = ftruncate(shmd, SHM_SIZE);
            if (result == -1) {
                std::cout << "Error al cambiar el tamaño!" << std::endl;
                throw 1;
            }

            std::cout << "Escribiendo en la Memoria Compartida!" << std::endl;
            char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

            if (ptr == MAP_FAILED) {
                std::cout << "Error al escribir en la Memoria Compartida!" << std::endl;
                throw 1;
            }

            // Esto debe de ser recibido por el usuario (cin) y se lo debemos de enviar al servidor para que lo guarde en un fichero (.txt)
            char information[] = "Haciendo una prueba, este es un mensaje enviado desde el servidor\n"; 
            memcpy(ptr, information, sizeof(information));

        }
        catch (...) {
            close(shmd);
            exit(1);
        }

        std::cin >> num;
    }
        close(shmd);
        shm_unlink(SHM_NAME);
    return 0;
}