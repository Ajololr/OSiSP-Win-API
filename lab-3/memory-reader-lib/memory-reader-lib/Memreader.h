#pragma once
#include <Windows.h>

extern "C" __declspec(dllexport) bool WINAPI findAndReplace(const char* target,const char* replacement);