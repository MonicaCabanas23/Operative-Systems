#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <regex>
#include <list>

using namespace std; 

enum command {Open=0, Kill=1, End=2, Error=-1};

// For saving each process information in a list
class Process {
    private:
        string path; 
        _pid_t pi;
        _pid_t parent;
    public: 
        Process(string, _pid_t, _pid_t);
        string getPath();
        void setPath(string);
};

// commands functions
void open(); 
void kill();
void end();
// entering commands
void enterCommands();

__pid_t parentPid = getpid();

int main() {

    enterCommands();

    return 0;
}

void open() {
    string path = "";

    _pid_t pid = fork(); // Creates the child process
    bool is_child_process = false;

    if (pid < 0) {
        cout << "Process creation failed" << "\n";
    }
    else if (pid == 0) {
        is_child_process = true;
        // path = getPath();
        // printPID(path);

        // execl(path.c_str(), nullptr);

    }
    else { // pid > 0
        wait(nullptr); // El proceso padre espera a que el proceso hijo termine
    }

    if (!is_child_process)
    {
        cout << "\nProceso Padre terminó Su Ejecución" << endl;
    }
}

void kill() {
    pid_t child_pid = getPID();
    auto ppid = getppid(); // Obtiene el id del proceso padre

    cout << "ppid: " << ppid << " parentId: " << parentPid << endl;

    if( ppid == parentPid) {
        kill(child_pid, SIGTERM);
        cout << "\nThe process " << child_pid << " has been killed!"; 
    }
    else {
        cout << "The process is not a child_process" << endl;
    }

}


void enterCommands(){
    // List of Processes
    list<Process> ProcessesList;
    command cmd;
    string strcmd = "";
    string argument = ""; 

    while(cmd != End) {
        cout << "\n----------------------------------------\nQueue of child processes: ";
        cout << "\n-----------------------------------------\nEnter your command: ";
        getline(cin, strcmd);

        if(regex_match(strcmd, regex("(open)(.*)"))) {
            cmd = Open;
            argument = strcmd.substr(strcmd.find(" ") + 1);
            // Hace falta hacer trim del argumento
        }
        else if(regex_match(strcmd, regex("(kill)(.*)"))){
            cmd = Kill;
            argument = strcmd.substr(strcmd.find(" ") + 1);
            // Hace falta hacer trim del argumento
        }
        else if(regex_match(strcmd, regex("(end)"))){
            cmd = End;
        }
        else {
            cmd = Error;
        }

        switch (cmd)
        {
        case 0:
            cout << "\nOpening " << argument;
            //open();
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        case 1:
            cout << "\nKilling " << argument;
            //kill();
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        case 2:
            cout << "\nEnding ";
            //end();
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        default:
            cout << "\nCommand not recognized. Try again. ";
            break;
        }
    }
     
}