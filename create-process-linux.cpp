#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

__pid_t parentPid = getpid();

std::string getPath() {
    std::string path;

    std::cout << "\nWrite the path of the program you want to execute (for example: /usr/bin/gnome-temrminal): ";
    std::cin >> path;

    return path;
}

void printPID(std::string path) {
    auto pid = getpid(); // Obtiene el id del proceso hijo

    std::cout << "\n----------------------------------------------------------\n";
    std::cout << "Process id of the current process: " << pid << "\n";
    std::cout << "Program executing currently: " << path << "\n";
    std::cout << "----------------------------------------------------------\n";
}

void open() {
    std::string path = "";
    auto pid = getpid();

    pid = fork(); // Crea el proceso hijo
    bool is_child_process = false;

    if (pid < 0) {
        std::cout << "Process creation failed" << "\n";
    }
    else if (pid == 0) {
        is_child_process = true;
        path = getPath();
        printPID(path);

        execl(path.c_str(), nullptr);

    }
    else { // pid > 0
        wait(nullptr); // El proceso padre espera a que el proceso hijo termine
    }

    if (!is_child_process)
    {
        std::cout << "\nProceso Padre terminó Su Ejecución" << std::endl;
    }
}

pid_t getPID() {
    pid_t pid;
    std::cout << "Write the PID of the process to kill: "; 
    std::cin >> pid;
}

void kill() {
    pid_t child_pid = getPID();
    auto ppid = getppid(); // Obtiene el id del proceso padre

    std::cout << "ppid: " << ppid << " parentId: " << parentPid << std::endl;

    if( ppid == parentPid) {
        kill(child_pid, SIGTERM);
        std::cout << "\nThe process " << child_pid << " has been killed!"; 
    }
    else {
        std::cout << "The process is not a child_process" << std::endl;
    }

}

void end() {

}

void commands() {
    int option = 0;

    do 
    {
        std::cout << "\nChoose your option: \n1. open \n2. kill \n3. end";
        std::cout << "\nYour option: ";
        std::cin >> option;

        switch (option)
        {
        case 1:
            open();
            break;
        case 2:
            kill();
            break;
        case 3:
            end();
            break;
        default:
            std::cout << "\nInvalid option";
            break;
        }
    } while (option != 3);
    
}

int main() {

    commands();

    return 0;
}