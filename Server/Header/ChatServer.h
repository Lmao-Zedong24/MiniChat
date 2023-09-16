#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <Network/Header.h>
#include "ChatDataS.h"

class ChatServer
{
public:
	ChatServer();

	int OpenServer();
	void CloseServer();
	void Run();
	//void InterpretReceive(int result, int index, const LibNetwork::ChatData& data, std::unordered_set<int>& closeSocketBuffer);

	inline static const TCHAR* NEW_CLIENT_MESSAGE = " has joined the server.";
	inline static const TCHAR* ONLY_CLIENTS_MESSAGE = "No Other Users Connected.";
	inline static const TCHAR* CURRENT_CLIENTS_MESSAGE = "Connected Users :";
	//inline static const char* ASK_NAME_MESSAGE = "Please enter UserName: ";
private:
	void SendDataToClients(ChatData& data, const uintptr_t& ignoreClients);
	void SendDataToClients(ChatData& data, const std::unordered_set<uintptr_t>& ignoreClients);
	void AddClient(const uintptr_t& socketId);
	void RemoveClient(const uintptr_t& socketId);
	void JoinNewClient(ChatData& data, const uintptr_t& socketId);

	std::unordered_map<uintptr_t, std::string> m_clientNames;
	std::vector<uintptr_t> m_clientSockets;
	LibNetwork::ServerEvents m_events;
	uintptr_t m_serverSockets[2] = {};

};