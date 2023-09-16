#include "InputOutput.h"

#include <Winsock2.h>
#include <iostream>
#include <tchar.h>
#ifndef UNICODE
#include <stdio.h> //printf
#endif

#pragma comment ( lib, "Ws2_32.lib" )

std::string GetUserInput()
{
	std::string str; //TODO: modify GetUserInput()
	std::getline(std::cin, str);
	return str;
}

void ReportError(int errorCode)
{
	LPTSTR buffer;
	DWORD tcharcount = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, 0, (LPTSTR)&buffer, 0, nullptr);

	if (tcharcount == 0) {
		DebugMessage(TEXT("ERROR: %d (fail to format message because error: %d)\r\n"), errorCode, GetLastError());
	}
	else {
		DebugMessage(buffer);
	}

	HLOCAL freeSuccess = LocalFree(buffer);
	if (freeSuccess != NULL) {
		DebugMessage(TEXT("fail to free buffer because error: %d\r\n"), GetLastError());
	}
}

void SaveCursor()
{
	ConsoleMessage(TEXT("\033[s"));
}

void RestoreCursor()
{
	ConsoleMessage(TEXT("\033[u"));
}

void EraseFoward()
{
	ConsoleMessage(TEXT("\033[J"));
}

void LineFeed()
{
	ConsoleMessage(TEXT("\n"));
}
