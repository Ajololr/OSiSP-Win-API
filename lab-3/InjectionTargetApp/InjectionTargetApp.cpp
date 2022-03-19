#include <iostream>
#include <thread>
#include <windows.h>

int main()
{
    char text[] = "test";
    long pid = GetCurrentProcessId();
    printf("%d \n", pid);
    while (true) {
        printf("%s ", text);
        Sleep(1000);
    }
}