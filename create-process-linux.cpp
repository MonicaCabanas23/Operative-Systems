#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <regex>
#include <list>
#include <algorithm>
#include <string>

using namespace std; 

enum command {Open=0, Kill=1, End=2, Error=-1};
bool is_child_process = false;

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
bool kill(pid_t, Process);
void end(list<Process>*);
Process getProcess(pid_t, list<Process>*);
void eraseFromList(Process, list<Process>*);
// entering commands
void enterCommands();

int main() {

    if (!is_child_process)
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

    if (pid < 0) {
        cout << "\nProcess creation failed" << "\n";
    }
    else if (pid == 0) {
        is_child_process = true;
        cout << "\n-----------------------------------------";
        cout << "\nChild pid: " << getpid();
        cout << "\nStarting child execution as a clone of the current program! ";

        execl(path.c_str(), path.c_str(), "", "", (char *)0 ); // change the child process image
    } else {
        int status;
        waitpid(pid, &status, 0);
        // Create new process object for later save it in the list
        Process newProcess = Process(path, pid, getpid());
        (*l).push_back(newProcess);
        cout << "\n-----------------------------------------";
        cout << "\nThe child changed its execution as a clone of this program to executing the path. We are in the parent ";
    }
}

void showList(list<Process>* l) {
    list<Process>::iterator it;

    for (it = (*l).begin(); it != (*l).end(); ++it){
        cout << "\n" << (*it).getPid() << " " << (*it).getPath();
    }
}

Process getProcess(pid_t pid, list<Process> *l) {
    list<Process>::iterator it;

    //Searching the process based on the pid
    for (it = (*l).begin(); it != (*l).end(); ++it){
        if(pid && pid == (*it).getPid()){
            return *it;
        }
    }
    // Process not found, return a default-constructed Process object
    return Process("", {0}, 0);
}

bool kill(pid_t child_pid, Process process) {
    auto ppid = getpid(); // Obtiene el id del proceso padre
    int exit_status;

    if( ppid == process.getParent()) {
        exit_status =  kill(child_pid, SIGKILL);
        cout << exit_status;
        if(exit_status == 0) {
            cout << "\nThe process " << child_pid << " has been killed!"; 
            return true;
        }
        else {
            cout << "\nAn error ocurred trying to kill the process. ";
            return false;
        }
    }
    else {
        cout << "The process is not a child_process" << endl;
        return false;
    }

}

void end(list<Process>* l) {
    list<Process>::iterator it;

    //Killing all processes
    if(!(*l).empty()) {
        for (it = (*l).begin(); it != (*l).end(); ++it){
            kill((*it).getPid(), *it);
        }
    }
}

void eraseFromList(Process process, list<Process>* l) {
    list<Process>::iterator it;

    // Searching for the index of the element
    for (it = (*l).begin(); it != (*l).end(); ++it) {
        if (process.getPid() == (*it).getPid()) break;
    }
    // Deleting the element from the list
    (*l).erase(it);
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
            if(kill(static_cast<pid_t>(stoi(argument)), getProcess(static_cast<pid_t>(stoi(argument)), &ProcessesList)))
                eraseFromList(getProcess(static_cast<pid_t>(stoi(argument)), &ProcessesList), &ProcessesList); 
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        case 2:
            cout << "\nEnding ";
            end(&ProcessesList);
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