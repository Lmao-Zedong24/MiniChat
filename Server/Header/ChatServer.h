#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <Network/Header.h>

class ChatServer
{
public:
	ChatServer();

	int OpenServer();
	int CloseServer();
	int Run();
	void SendDataToClients(const LibNetwork::TCPData& data, const uintptr_t& ignoreClients);
	void SendDataToClients(const LibNetwork::TCPData& data, const std::unordered_set<uintptr_t>& ignoreClients);
	//void InterpretReceive(int result, int index, const LibNetwork::TCPData& data, std::unordered_set<int>& closeSocketBuffer);

	inline static const int NUM_CLIENTS = 2; //TODO: put good number here
	inline static const char* NEW_CLIENT_MESSAGE = " has joined the server.";
	//inline static const char* ASK_NAME_MESSAGE = "Please enter UserName: ";
public:
	std::unordered_map<uintptr_t, std::string> m_clientNames;
	std::vector<uintptr_t> m_clientSockets;
	LibNetwork::ServerEvents m_events;
	uintptr_t m_serverSocket;

};