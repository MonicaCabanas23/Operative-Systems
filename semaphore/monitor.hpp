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
    const unsigned SHM_PERMISSION = 00600;
    const unsigned SHM_SIZE = 500;
    const char SHM_NAME[11]{"/IBERO_Shm"};
    const char SEM_NAME [10]{"/SemIBERO"};
    const unsigned SEM_PERM = 0660; // allow owner and group to read and write
    sem_t* sem;
    int shmd;
    bool _server;
public:
    Monitor();
    ~Monitor();

    int Inicializar(bool server);
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

    // if it is the server then destroy the shared memory
    if (_server) {
        shm_unlink(SHM_NAME);
        cout << "\nShared memory destroyed";
        sem_unlink(SEM_NAME);
        cout << "\nSemaphore destroyed" << endl;
    }
}

int Monitor::Inicializar(bool server) {
    _server = server;

    if (_server)
    {
        // Shared memory creation
        shmd = shm_open(SHM_NAME, O_CREAT|O_RDWR, SHM_PERMISSION);
        ftruncate(shmd, SHM_SIZE);

        // Semaphore creation
        sem = sem_open(SEM_NAME, O_CREAT, SEM_PERM, /* Initial value */ 1);

        // Close shared memory
        close(shmd);
    }
    else
    {
        // Opening shared memory
        shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION);
        if(shmd == -1) {
            cout << "\nShared memory has not been created yet.";
            return -1;
        }

        // Opening semaphore
        sem = sem_open(SEM_NAME, 0);
        if (sem == SEM_FAILED) {
            cout << "\nSemaphore has not been created yet" << endl;
            return -1;
        }

        close(shmd);
    }

    return 0;
}

int Monitor::writeInMemory(string message) {

    sem_wait(sem);
    // Open the shared memory
    shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION);

    try {
        cout << "\nWriting message in shared memory" << endl;
        char* ptr = (char*) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shmd, 0);

        if (ptr == MAP_FAILED) {
            cout << "\nError when trying to write in the shared memory" << endl;
            throw 1;
        }

        // Copying the message from the *src (message.c_str()) to the *dest (ptr)
        memcpy(ptr, message.c_str(), message.length());  
    }
    catch (...) {
        close(shmd);
        sem_post(sem); // before ending the program, release the semaphore
        return -1;
        exit(1);
    }

    // Closing the shared memory and freeing the semaphore
    close(shmd);
    sem_post(sem);
    return 0;
}

int Monitor::readMemory(void (*callback) (string)) {
    struct stat shmobj_st;

    sem_wait(sem);
    // Open the shared memory
    shmd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION);

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
        
        cout << "\nMessage received: "<< ptr;
        
        // Check if the shared memory contains data (non-empty)
        size_t dataLength = strlen(ptr);
        if (dataLength > 0)
            callback(ptr);
    }
    catch (...) {
        close(shmd);
        sem_post(sem); // before ending the program, release the semaphore
        return -1;
    }

    // Closing the shared memory and freeing the semaphore
    close(shmd);
    if (sem_post(sem) == -1) {
        perror("sem_post");
    }

    return 0;
    
}