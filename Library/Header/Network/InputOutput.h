#pragma once
#include <string>
#include <tchar.h>

#if 0
#include <tchar.h>
#define DebugMessage(...)  _tprintf(__VA_ARGS__)
#else 
#define DebugMessage(...)
#endif

#define ConsoleMessage(...) _tprintf(__VA_ARGS__)

std::string GetUserInput();
void ReportError(int errorCode);

void SaveCursor();
void RestoreCursor();
void EraseFoward();
void LineFeed();
