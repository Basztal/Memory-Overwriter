#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

using namespace std;

HANDLE hProc = NULL;
DWORD pID;

string errorPrefix = "[!] ";

bool attachProc(char* procName)
{
    PROCESSENTRY32 procEntry32;

    // Defining the size so we can populate it
    procEntry32.dwSize = sizeof(PROCESSENTRY32);

    // Taking a snapshot of all processes running
    cout << "Taking a snapshot of all processes running..." << endl << endl;
    auto hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // Checking if the snapshot of list is taken 
    if (hProcSnap == INVALID_HANDLE_VALUE) {
        cout << errorPrefix << "Failed to take snapshot of processes!" << endl;
        return false;
    }

    // Looping overwrite process (if you dont want to loop, delete it)
    while (Process32Next(hProcSnap, &procEntry32)) {
        cout << procEntry32.szExeFile << endl;

        // Lookiing for process
        if (!strcmp(procName, procEntry32.szExeFile)) {
            cout << "Found process " << procEntry32.szExeFile << " with process ID " << procEntry32.th32ProcessID << "." << endl;
            // Getting access to the process
            hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
            pID = procEntry32.th32ProcessID;
            cout << "You now have access to this process." << endl;

            if (hProc == NULL) {
                cout << errorPrefix << "Failed getting handle to process." << endl;
            }

            CloseHandle(hProcSnap);
            return true;

        }
    }

    // If program can't find the process returns error and close handle
    cout << errorPrefix << "Couldn't find " << procName << " in the process snapshot." << endl;
    CloseHandle(hProcSnap);

    return false;
}

// Write process memory class
template <class dataType>
void wpm(dataType valToWrite, DWORD adressToWrite)
{
    WriteProcessMemory(hProc, (PVOID)adressToWrite, &valToWrite, sizeof(dataType), 0);
}

// Read process memory class
template <class dataType>
dataType rpm(DWORD addressToRead)
{
    dataType rpmBuffer;

    ReadProcessMemory(hProc, (PVOID)addressToRead, &rpmBuffer, sizeof(dataType), 0);

    return rpmBuffer;
}

int main()
{   
    // Example - Overwriting Timberman score

    // Process name
    attachProc((char*)"Timberman.exe");

    // 0x + memory adress
    DWORD memoryAdress = 0x19EE41D0;

    while (true)
    {
        // Type of variable + variable you want to overwrite
        wpm<int>(69, memoryAdress);
    }
}




