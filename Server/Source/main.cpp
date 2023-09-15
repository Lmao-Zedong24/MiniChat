//main.cpp 
//Server

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _UNICODE
#undef _UNICODE
#endif

#if 0
#define UNICODE
#define _UNICODE
#endif


#define WIN32_LEAN_AND_MEAN

#include <Network/Header.h>
#include <Network/InputOutput.h>
#include <windows.h>
#include <winnt.h>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "ChatServer.h"

constexpr const TCHAR* DefaultName = "Guest";

int main()
{
	ChatServer server;
	int result;
	if ((result = server.OpenServer()) != EXIT_SUCCESS)
		return result;

	server.Run();

	if ((result = server.CloseServer()) != EXIT_SUCCESS)
		return result;

	return EXIT_SUCCESS;
}