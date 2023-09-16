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

#include "ChatServer.h"

int main()
{
	ChatServer server;
	int result;
	if ((result = server.OpenServer()) != FUNC_SUCCESS)
		return result;

	server.Run();
	server.CloseServer();

	return EXIT_SUCCESS;
}