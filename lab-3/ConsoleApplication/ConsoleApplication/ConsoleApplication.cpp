#include <iostream>
#include <stdio.h>
#include <new>
#include <thread>
#include <windows.h>

#define dllName "memory-reader-lib.dll"
#define kernelDll "Kernel32.dll"
#define loadLibFunc "LoadLibraryA"

const char* target = "test";
const char* replacement = "ilya androsov";

typedef HMODULE(WINAPI* LPLoadLibrary)(LPCSTR);

// Static import 
extern "C" _declspec(dllexport) bool WINAPI findAndReplace(const char* target,const char* replacement);

char* buf = new char[strlen(target) + 1];

void injectDll(DWORD injectPid) {
    // Load kernel32 lib in process
    HMODULE hdll = LoadLibraryA(kernelDll);

    // Get loadlibA func pointer
    LPLoadLibrary LoadLibraryA = (LPLoadLibrary)GetProcAddress(hdll, loadLibFunc);

    // Get process descriptor
    HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
        PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS,
        FALSE, injectPid);

    // Alloc mem for injection dll name (pointer)
    LPVOID path = VirtualAllocEx(hProc, NULL, strlen(dllName) + 1,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Write dll name
    WriteProcessMemory(hProc, path, dllName,
        strlen(dllName) + 1, NULL);

    DWORD threadID;

    // Create remote thread
    HANDLE HThread = CreateRemoteThread(hProc, NULL, NULL,
        (LPTHREAD_START_ROUTINE)LoadLibraryA, (LPVOID)path, NULL, &threadID);

    if (HThread != NULL) {
        // Wait until thread completion
        WaitForSingleObject(HThread, INFINITE);
    }
    else {
        printf("error");
    }

    FreeLibrary(hdll);
    CloseHandle(hProc);

}

int main()
{
    bool res;
    strcpy_s(buf, strlen(target) + 1, target);

    printf("Old string: %s\n", buf);

    // Dynamic import 
    //HMODULE hlib = LoadLibrary(L"memory-reader-lib.dll");
    //if (hlib != NULL) {
    //    bool (WINAPI * findAndReplace)(const char*, const char*);
    //    findAndReplace = (bool (WINAPI*)(const char*, const char*))GetProcAddress((HMODULE)hlib, "_findAndReplace@8");
    //    res = findAndReplace(target, replacement);

    //    FreeLibrary(hlib);
    //}
    //bool (WINAPI* findAndReplace)(const char*, const char*);
    findAndReplace(target, replacement);
    printf("New string: %s\n", buf);

    DWORD pid;
    printf("Enter process pid: ");
    scanf_s("%d", &pid);

    
    injectDll(pid);
}