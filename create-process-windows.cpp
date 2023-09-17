#include <windows.h>
#include <stdio.h>
#include <tchar.h>
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
        PROCESS_INFORMATION pi;
        DWORD parent;
    public: 
        Process(string, PROCESS_INFORMATION, DWORD);
        string getPath();
        PROCESS_INFORMATION getPi();
        DWORD getParent();
        void setPath(string);
        void setPi(PROCESS_INFORMATION);
        void setParent(DWORD);
};

// command functions 
void open(string, list<Process>*);
bool kill(Process); 
void endProgram(list<Process>*);
// necessary functions for executing commands 
void showList(list<Process>*);
Process getProcess(DWORD, list<Process>*);
void eraseFromList(Process, list<Process>*);
// bucle
void enterCommands(); 

int main( ) {

    enterCommands();

    return 0;
}

// Initialize a process object
Process::Process(string path, PROCESS_INFORMATION pi, DWORD parent) {
    this->path = path;
    this->pi = pi;
    this->parent = parent;
}

// getters
string Process::getPath() {
    return path;
}

PROCESS_INFORMATION Process::getPi() {
    return pi;
}

DWORD Process::getParent() {
    return parent;
}

// setters
void Process::setPath(string path) {
    this->path = path;
}

void Process::setPi(PROCESS_INFORMATION pi) {
    this->pi = pi;
}

void Process::setParent(DWORD parent) {
    this->parent = parent;
}

// creating a new process
void open(string strPath, list<Process>* l) {
    HANDLE hProcess; // puntero al proceso localizado en el GC heap
    HANDLE hThread;  // puntero al hilo localizado en el GC heap
    STARTUPINFO si;  // si es una estructura que contiene las especificaciones (propiedades) que tendrá la ventana para el nuevo proceso que se creará
    PROCESS_INFORMATION pi; // pi es una estructura que contendrá información del proceso a crear: hProcess, hTread, dwProcessId, dwTrheadId
    ZeroMemory(&si, sizeof(si)); // limpiar la dirección de memoria &si del tamaño de si 
    ZeroMemory(&pi, sizeof(pi)); // limpiar la dirección de memoria &pi del tamaño de pi 

    LPCSTR path = _T(strPath.c_str());
    
    // Parent process ID
    DWORD parentId = GetCurrentProcessId();

    // Creating child process
    if(!CreateProcess(
        path,  // program to execute
        NULL, 
        NULL, 
        NULL,
        FALSE, 
        0, 
        NULL, 
        NULL, 
        &si, 
        &pi
    )) {
        cout << "\n-----------------------------------------";
        cout << "\nProcess creation failed: " << GetLastError(); 
    } else {
        cout << "\n-----------------------------------------";
        cout << "\nProcess created! ";

        Process newProcess = Process(strPath, pi, parentId);
        (*l).push_back(newProcess);

        /* if(TerminateProcess(getProcess(newProcess.getPi().dwProcessId, l).getPi().hProcess, 0)){
            cout << "\nProcess Terminated";
        } */

        // WaitForSingleObject(pi.hProcess, INFINITE); // parent process waits for the child process
    }
}

void showList(list<Process>* l) {
    list<Process>::iterator it;

    for (it = (*l).begin(); it != (*l).end(); ++it){
        cout << "\n" << (*it).getPi().dwProcessId << " " << (*it).getPath();
    }
}

// terminating a process by first evaluating if it is a chill process
bool kill(Process process) {
    DWORD parent = GetCurrentProcessId();

    if (process.getParent() == parent) {
        HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, process.getPi().dwProcessId);
        
        if (processHandle != NULL) {
            if (TerminateProcess(processHandle, 0)) {
                cout << "\nProcess " << process.getPi().dwProcessId <<" terminated successfully.";

                CloseHandle(process.getPi().hProcess);
                CloseHandle(process.getPi().hThread);
                return true;
            }
            else {
                cout << "\nFailed to terminate process.";
                return false;
            }
            CloseHandle(processHandle);
    }
    else {
        cout << "\nFailed to obtain process handle.";
        return false;
    }
    }
    else {
        cout << "\nNo child process identified. ";
        return false;
    }
}

Process getProcess(DWORD pid, list<Process> *l) {
    list<Process>::iterator it;

    //Searching the process based on the pid
    for (it = (*l).begin(); it != (*l).end(); ++it){
        if(pid && pid == (*it).getPi().dwProcessId){
            return *it;
        }
    }
    // Process not found, return a default-constructed Process object
    return Process("", {0}, 0);
}

void eraseFromList(Process process, list<Process>* l) {
    list<Process>::iterator it;

    // Searching for the index of the element
    for (it = (*l).begin(); it != (*l).end(); ++it) {
        if (process.getPi().dwProcessId == (*it).getPi().dwProcessId) break;
    }
    // Deleting the element from the list
    (*l).erase(it);
}

void endProgram(list<Process>* l) {
    list<Process>::iterator it;

    //Killing all processes
    if(!(*l).empty()) {
        for (it = (*l).begin(); it != (*l).end(); ++it){
            kill(*it);
        }
    }
}

void enterCommands(){
    // List of Processes
    list<Process> ProcessesList;
    char* end;
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
            // First make sure the list is not empty, then make sure the process has been terminated for erasing from the list
            if(!ProcessesList.empty() && kill(getProcess(static_cast<DWORD>(stoul(argument)), &ProcessesList))) 
                eraseFromList(getProcess(static_cast<DWORD>(stoul(argument)), &ProcessesList), &ProcessesList);
            // Cleaning variables
            strcmd = "";
            argument = "";
            break;
        case 2:
            cout << "\nEnding ";
            endProgram(&ProcessesList);
            // Making sure the list is not empty
            if(!ProcessesList.empty())
                eraseFromList(getProcess(static_cast<DWORD>(stoul(argument)), &ProcessesList), &ProcessesList);
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