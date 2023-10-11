#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

class Monitor{
private:
    const unsigned SHM_PERMISSION = S_IRUSR | S_IWUSR;
    const unsigned SHM_SIZE = 5;
    const char SHM_NAME[11]{"/IBERO_Shm"};
    const char SEM_NAME [10]{"/SemIBERO"};
    const unsigned SEM_PERM = 0660; // allow owner and group to read and write
    sem_t* sem;
    int shmd;
    bool _server;
public:
    Monitor();
    ~Monitor();

    void Inicializar(bool server);
    int writeInMemory(string);
    int readMemory(void (*callback)(string));
};

Monitor::Monitor() {
    /* Inicializando el Monitor */
    _server = false;
}

Monitor::~Monitor() {
    // close and delete semaphore
    sem_close(sem);
    sem_unlink(SEM_NAME);

    // if it is the server then destroy the shared memory
    if (_server)
        shm_unlink(SHM_NAME);
    cout << "\nShared memory and semaphore destroyed";
}

void Monitor::Inicializar(bool server) {
    _server = server;

    if (_server)
    {
        cout << "\nCreando";
        // Shared memory creation
        shmd = shm_open(SHM_NAME, O_CREAT|O_RDWR, SHM_PERMISSION);
        ftruncate(shmd, SHM_SIZE);

        // Semaphore creation
        sem_t *sem = sem_open(SEM_NAME, O_CREAT, SEM_PERM, /* Initial value */ 1);

        // Close shared memory
        close(shmd);
    }
    else
    {
        sleep(1); /* Dando tiempo a que el proceso server pueda crear la Memoria Compartida */
        shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION);
        sem = (sem_t*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);
        close(shmd);
    }
}

int Monitor::writeInMemory(string message) {
    sem_wait(sem);

    try {
        cout << "\nWriting message in shared memory" << endl;
        char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

        if (ptr == MAP_FAILED) {
            cout << "\nError when trying to write in the shared memory" << endl;
            throw 1;
        }

        // Copying the message from the *src (message.c_str()) to the *dest (ptr)
        memcpy(ptr, message.c_str(), sizeof(message));  
        return 0;
    }
    catch (...) {
        close(shmd);
        sem_post(sem); // before ending the program, release the semaphore
        return -1;
        exit(1);
    }

    sem_post(sem);
}

int Monitor::readMemory(void (*callback) (string)) {
    struct stat shmobj_st;
    if(_server) {
        sem_wait(sem);
        
        try {
            
            if (fstat(shmd, &shmobj_st) == -1) {
                cout << "\nError when obtaining features from the shared memory";
                throw 1;
            }

            cout << "\nReading message from the shared memory";
            char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
            if (ptr == MAP_FAILED) {
                cout << "\nError trying to read the shared memory";
                throw 1;
            }
            
            cout << "\nMessage received from client: "<< ptr;
            
            // Check if the shared memory contains data (non-empty)
            size_t dataLength = strlen(ptr);
            if (dataLength > 0)
                callback(ptr);
        }
        catch (...) {
            close(shmd);
            sem_post(sem); // before ending the program, release the semaphore
            return -1;
            exit(1);
        }

        sem_post(sem);
        return 0;
    } else {
        sem_wait(sem);

        try {
            
            if (fstat(shmd, &shmobj_st) == -1) {
                cout << "\nError when obtaining features from the shared memory";
                throw 1;
            }

            cout << "\nReading message from the shared memory";
            char* ptr = (char*) mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
            if (ptr == MAP_FAILED) {
                cout << "\nError trying to read the shared memory";
                throw 1;
            }
            
            cout << "\nMessage received from server: "<< ptr;
            
            // Check if the shared memory contains data (non-empty)
            size_t dataLength = strlen(ptr);
            if (dataLength > 0)
                callback(ptr);
        }
        catch (...) {
            close(shmd);
            sem_post(sem); // before ending the program, release the semaphore
            return -1;
            exit(1);
        }

        sem_post(sem);
        return 0;
    }
    
}