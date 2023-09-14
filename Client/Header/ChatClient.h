//ChatClient.h
#pragma once

#include <string>
#include <Network/Header.h>

class ChatClient
{
public:
	ChatClient();

	int OpenClient();
	int CloseClient();
	void Run();

	void ReceiveData();
	int SendClientName();
	int SendClientMessage();

	static inline const int MAX_NAME_LENGTH = 10;
	static inline const std::string MESSAGE_SEPARATOR = "> ";
private:
	uintptr_t m_clientSocket;
	LibNetwork::TCPData m_data;
	std::string m_name;
	size_t m_offset;
	LibNetwork::ClientEvents m_events;
};