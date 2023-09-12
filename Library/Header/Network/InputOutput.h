#pragma once

#include <string>

#if 1
#include <tchar.h>
#define DebugMessage(...)  _tprintf(__VA_ARGS__)
#else 
#define DebugMessage(...)
#endif

#define ConsoleMessage(...) _tprintf(__VA_ARGS__)

std::string GetUserInput();
void ReportError(int errorCode);
