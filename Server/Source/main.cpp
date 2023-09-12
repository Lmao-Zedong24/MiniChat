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


	//for (int i = 0; i < ChatServer::NUM_CLIENTS; i++){ //need 2 to continue
	//	server.m_clientSockets.push_back(0);
	//	result = Accept(server.m_serverSocket, server.m_clientSockets.back());
	//	if (result != EXIT_SUCCESS)
	//		return result;
	//}

	//while (!server.m_clientSockets.empty()) { //main loop
	//	std::unordered_set<int> closeSocketBuffer;
	//	TCPData data;
	//	for (int i = 0; i < server.m_clientSockets.size(); i++) {
	//		result = Receive(server.m_clientSockets[i], data);
	//		server.InterpretReceive(result, i, data, closeSocketBuffer);
	//	}

	//	for (auto& closeSocket : closeSocketBuffer) { //close clientSockets
	//		if ((result = CloseSocket(closeSocket)) != EXIT_SUCCESS)
	//			return result;
	//	}
	//}
}