#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <regex>
#include <list>
#include <algorithm>
#include <string>

using namespace std; 

enum command {Open=0, Kill=1, End=2, Error=-1};

// For saving each process information in a list
class Process {
    private:
        string path; 
        pid_t pid;
        pid_t parent;
    public: 
        Process(string, pid_t, pid_t);
        string getPath();
        pid_t getPid(); 
        pid_t getParent();
        void setPath(string);
        void setPid(pid_t);
        void setParent(pid_t);
};

// commands functions
void open(string, list<Process>*); 
// void kill();
// void end();
// entering commands
void enterCommands();

int main() {

    enterCommands();

    return 0;
}

Process::Process(string path, pid_t pid, pid_t parent) {
    this->path = path; 
    this->pid = pid; 
    this->parent = parent;
}

string Process::getPath() {
    return path;
}

pid_t Process::getPid() {
    return pid; 
}

pid_t Process::getParent() {
    return parent;
}

void Process::setPath(string path) {
    this->path = path;
}

void Process::setPid(pid_t pid) {
    this->pid = pid;
}

void Process::setParent(pid_t parent) {
    this->parent = parent;
}

void open(string path, list<Process>* l) {
    pid_t pid;

    pid = fork(); // Creates the child process
    bool is_child_process = false;

    if (pid < 0) {
        cout << "\nProcess creation failed" << "\n";
    }
    else if (pid == 0) {
        is_child_process = true;

        cout << "\n-----------------------------------------";
        cout << "\nProcess created! ";

        // Create new process object for later save it in the list
        Process newProcess = Process(path, getpid(), getppid());
        (*l).push_back(newProcess);

        // execl(path.c_str(), path.c_str(), "", "", (char *)0 ); // change the child process image

    }

    if (!is_child_process)
    {
        cout << "\nProceso Padre terminó Su Ejecución" << endl;
    }
}

void showList(list<Process>* l) {
    list<Process>::iterator it;

    for (it = (*l).begin(); it != (*l).end(); ++it){
        cout << "\n" << (*it).getPid() << " " << (*it).getPath();
    }
}

void kill(pid_t child_pid) {
    auto ppid = getppid(); // Obtiene el id del proceso padre
    cout << "\npid del proceso actual: " << getpid() << "\nPpid: " << ppid;

    //if( ppid == parentPid) {
        //kill(child_pid, SIGTERM);
        //cout << "\nThe process " << child_pid << " has been killed!"; 
    //}
    //else {
        //cout << "The process is not a child_process" << endl;
    //}

}

void enterCommands(){
    // List of Processes
    list<Process> ProcessesList;
    command cmd;
    string strcmd = "";
    string argument = ""; 

    while(cmd != End) {
        cout << "\n----------------------------------------\nQueue of child processes: ";
        if(!ProcessesList.empty()) showList(&ProcessesList);
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
            open(argument, &ProcessesList);
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        case 1:
            cout << "\nKilling " << argument;
            kill(static_cast<pid_t>(stoi(argument)));
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