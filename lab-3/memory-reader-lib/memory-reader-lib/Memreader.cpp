#include "Memreader.h"
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

extern "C" __declspec(dllexport) bool WINAPI findAndReplace(const char* target,const char* replacement)
{
    bool wasFound = false;
    size_t targetLen = strlen(target);
    size_t valueLen = strlen(replacement);

    // Contains information about the current computer system
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // Contains information about a range of pages in the virtual address space of a process
    MEMORY_BASIC_INFORMATION info;
    // A pointer to the lowest memory address accessible to applications and dynamic-link libraries (DLLs).
    auto baseAddress = (LPSTR)si.lpMinimumApplicationAddress;

    // Highest memory address accessible to applications and DLLs
    while (baseAddress < si.lpMaximumApplicationAddress) {
        // Retrieves information about a range of pages in the virtual address space of the calling process
        if (VirtualQuery(baseAddress, &info, sizeof(info)) == sizeof(info)) {
            // State - The state of the pages in the region
            // AllocationProtect - The memory protection option when the region was initially allocated
            if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE) {
                // A pointer to the base address of the region of pages.
                baseAddress = (LPSTR)info.BaseAddress;
                // The size of the region beginning at the base address in which all pages have identical attributes, in bytes.
                char* memory = (char*)malloc(info.RegionSize);
                SIZE_T bytesRead;
 
                if (ReadProcessMemory(GetCurrentProcess(), baseAddress, memory, info.RegionSize, &bytesRead)) {
                    for (SIZE_T i = 0; i < bytesRead - targetLen; i++) {
                        if (strcmp(baseAddress + i, target) == 0) {
                            memcpy(baseAddress + i, replacement, valueLen + 1);
                            wasFound = true;
                        }
                    }
                }
                free(memory);
            }
        }
        baseAddress += info.RegionSize;
    }
    return wasFound;
}
