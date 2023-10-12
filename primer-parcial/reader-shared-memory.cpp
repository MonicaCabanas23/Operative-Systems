#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define SHM_NAME "/IBERO_Shm"
#define SHM_PERMISSION 00400
#define SHM_SIZE 500

int main() {
    std::cout << "Abriendo la Memoria Compartida!" << std::endl;
    int shmd = shm_open(SHM_NAME, O_RDONLY, SHM_PERMISSION);
    if (shmd == -1) {
        std::cout << "Error abriendo la Memoria Compartida!" << std::endl;
        exit(1);
    }

    try {
        struct stat shmobj_st;
        if (fstat(shmd, &shmobj_st) == -1)
        {
            std::cout << "Error al tomar las propiedades de la Memoria Compartida!" << std::endl;
            throw 1;
        }

        std::cout << "Leyendo de la Memoria Compartida!" << std::endl;
        char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "Error al leer en la Memoria Compartida!" << std::endl;
            throw 1;
        }
        std::cout << "Contenido de la Memoria Compartida: "<< ptr << std::endl;  // Esto se lo mostramos al usuario, entonces esto debe de ir en el cliente
    }
    catch (...) {
        close(shmd);
        exit(1);
    }

    close(shmd);
    shm_unlink(SHM_NAME);
    return 0;
}