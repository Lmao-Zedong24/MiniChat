//main.cpp 
//Client

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

#include <Network/Header.h>
#include <Network/InputOutput.h>
#include <stdlib.h>
#include <windows.h>
#include <winnt.h>
#include <string>
#include <iostream>
#include "ChatClient.h"

int main() 
{
	int result;
	ChatClient client;

	if ((result = client.OpenClient()) != FUNC_SUCCESS)
		return result;

	client.Run();
	
	if ((result = client.CloseClient()) != FUNC_SUCCESS)
		return result;

	return EXIT_SUCCESS;
}