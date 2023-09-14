#include "Header.h"
#include "NetworkErrors.h"
#include "InputOutput.h"
#include <vector>

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
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <tchar.h>
#ifndef UNICODE
#include <stdio.h> //printf
#endif

#pragma comment ( lib, "Ws2_32.lib")


namespace LibNetwork {

	int Initialize()
	{
		BYTE majorVersion = 2;
		BYTE minorVersion = 2;

		WORD versionRequested = MAKEWORD(majorVersion, minorVersion);
		WSADATA data;

		int result = WSAStartup(versionRequested, &data);

		if (result != 0) {
			ReportError(result);
			return EXIT_WSAStartup;
		}

		if (LOBYTE(data.wVersion) == majorVersion &&
			HIBYTE(data.wVersion) == minorVersion) {
			DebugMessage("Initialize successfull\n");
		}
		else {
			DebugMessage("Winsock incompatible\n");
		}

		return EXIT_SUCCESS;
	}

	int Disconnect()
	{
		int result = WSACleanup();

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_WSACleanup;
		}

		DebugMessage("Disconnect successfull\n");
		return EXIT_SUCCESS;
	}

	int CreateSocket(uintptr_t& socketId)
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (s == INVALID_SOCKET){
			ReportError(WSAGetLastError());
			return EXIT_socket;
		}
		socketId = (uintptr_t)s;

		DebugMessage("CreateSocket successfull\n");
		return EXIT_SUCCESS;
	}

	int CloseSocket(const uintptr_t& socketId)
	{
		int result = closesocket((SOCKET)socketId);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_closesocket;
		}

		DebugMessage("CloseSocket successfull\n");
		return EXIT_SUCCESS;
	}

	int BindSocket(const uintptr_t& serverSocket)
	{
		sockaddr_in name;
		name.sin_family = AF_INET;
		InetPton(AF_INET, TEXT("127.0.0.1"), &name.sin_addr.S_un.S_addr); // 127.0.0.1 is self
		name.sin_port = htons(PORT);

		int result = bind((SOCKET)serverSocket, (sockaddr*) &name, sizeof(name));

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_bind;
		}

		DebugMessage("BindSocket successfull\n");
		return EXIT_SUCCESS;
	}

	int Listen(const uintptr_t& serverSocket, int backlog = SOMAXCONN)
	{
		int result = listen((SOCKET)serverSocket, backlog);
		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_listen;
		}

		DebugMessage("Listen successfull\n");
		return EXIT_SUCCESS;
	}

	int Accept(const uintptr_t& serverSocket, uintptr_t& clientSocket)
	{
		SOCKET acceptSocket = accept((SOCKET)serverSocket, nullptr, nullptr);

		if (acceptSocket == INVALID_SOCKET) {
			ReportError(WSAGetLastError());
			return EXIT_listen;
		}

		clientSocket = (long long)acceptSocket;
		DebugMessage("Accept successfull\n");
		return EXIT_SUCCESS;
	}

	int Connect(const uintptr_t& clientSocket)
	{
		sockaddr_in name;
		name.sin_family = AF_INET;
		InetPton(AF_INET, TEXT("127.0.0.1"), &name.sin_addr.S_un.S_addr); // 127.0.0.1 is self
		name.sin_port = htons(PORT);

		int result = connect((SOCKET)clientSocket, (sockaddr*)&name, sizeof(name));

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_listen;
		}

		DebugMessage("Connect successfull\n");
		return EXIT_SUCCESS;
	}

	int Send(const uintptr_t& socketId, const TCPData& data)
	{
		int result = send(socketId, (const char*)&data, sizeof(TCPData), 0);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_send;
		}

		DebugMessage("Send successfull:\t");
		DebugMessage(data.buffer);
		DebugMessage("\n");

		return EXIT_SUCCESS;
	}

	int Receive(const uintptr_t& socketId, const TCPData& data)
	{
		int result = recv(socketId, (char*)&data, sizeof(TCPData), 0);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_recv;
		}

		if (result == 0) {
			DebugMessage("Connection gracefully closed\n");
			return EXIT_connectionClosed;
		}

		DebugMessage("Receive successfull\t");
		DebugMessage(data.buffer);
		DebugMessage("\n");

		return EXIT_SUCCESS;
	}

	TCPData& TCPData::WriteInBuffer(const char* info, size_t offset)
	{
		size_t infoLen = strlen(info);
		size_t bufMax = (size_t)GetBufferSize();
		size_t bufLen = bufMax - offset - 1;

		memcpy_s(&(buffer[offset]), bufLen, info, std::min<size_t>(bufLen, infoLen));
		buffer[std::min<size_t>(bufMax, infoLen + offset)] = '\0';

		return *this;
	}

	size_t TCPData::GetBufferSize()
	{
		return ARRAYSIZE(TCPData::buffer);
	}


	ServerEvents::ServerEvents()
	{
	}

	int ServerEvents::WaitForEvents()
	{
		static const int WAIT_TIME_MS = -1;

		int result = WSAPoll(m_events.data(), (ULONG)m_events.size(), (INT)WAIT_TIME_MS);
		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_WSAPool;
		}

		DebugMessage("Wait successfull\n");
		if (result == 0) {
			DebugMessage("Nothing Waited\n");
		}

		return result;
	}

	const uintptr_t& ServerEvents::GetSocketId(const int& index)
	{
		return m_events[index].fd;
	}

	int ServerEvents::AddServerEvent(const uintptr_t& serverId)
	{
		if (!m_events.empty())
			return EXIT_AddServerEvent;

		this->AddClientEvent(serverId);
		return EXIT_SUCCESS;
	}


	void ServerEvents::AddClientEvent(const uintptr_t& socketId)
	{
		if (m_unusedEventBuffer.empty()) {
			DebugMessage("Emplace event successfull\n");
			m_events.emplace_back((SOCKET)socketId, (SHORT)POLLRDNORM, (SHORT)0);
		}
		else {
			DebugMessage("Replace event successfull\n");
			m_events[*m_unusedEventBuffer.begin()] = { socketId, POLLRDNORM, 0 };
			m_unusedEventBuffer.erase(m_unusedEventBuffer.begin());
		}
	}

	void ServerEvents::RemoveEvent(const uintptr_t& socketId)
	{
		for (int i = 0; i < m_events.size(); i++) {
			if (m_events[i].fd == socketId) {
				m_unusedEventBuffer.emplace(i);
				m_events[i].fd = (SOCKET)-1;	//ignore when pooling
				DebugMessage("Remove successfull\n");
				return;
			}
		}

		DebugMessage("No Remove\n");
	}

	int ServerEvents::EvaluateEvents(uintptr_t& socketId, LibNetwork::TCPData& data, int& index)
	{
		int result;
		if (index == 0 && m_events[0].revents & POLLRDNORM) { //server socket
			socketId = m_events[index].fd;
			return (result = Accept(m_events[0].fd, socketId)) == EXIT_SUCCESS? 1 : -1;
		}

		for (index; index < m_events.size(); index++) { //clients
			if (m_events[index].revents & (POLLERR | POLLHUP | POLLRDNORM)) {
				socketId = m_events[index].fd;
				return (result = Receive(socketId, data)) == EXIT_SUCCESS? 0: -1; //same as return Receive
			}
		}

		index++;
		DebugMessage("End of Events\n");
		return -1;
	}

	enum class ClientEvents::EventType {
		Client,
		Console
	};

	ClientEvents::ClientEvents() : m_lpBuffer()
	{}

	int ClientEvents::AddClientEvent(const uintptr_t& socketId)
	{
		HANDLE newEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (newEvent == NULL) {
			ReportError(WSAGetLastError());
			return EXIT_AddClientEvent;
		}
		int result = WSAEventSelect(socketId, newEvent, FD_READ | FD_WRITE);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_AddClientEvent;
		}
		m_events.emplace_back(std::move(newEvent));
		m_typeEventsBuffer.emplace_back(ClientEvents::EventType::Client);

		int index = (int)m_events.size() - 1;
		m_clientIds.emplace(index, socketId);

		return index;
	}

	int ClientEvents::AddConsoleEvent()
	{
		HANDLE newEvent = GetStdHandle(STD_INPUT_HANDLE);

		if (newEvent == INVALID_HANDLE_VALUE) {
			ReportError(WSAGetLastError());
			return EXIT_AddConsoleEvent;
		}
		DebugMessage("AddConsoleEvent\n");
		m_events.emplace_back(std::move(newEvent));
		m_typeEventsBuffer.emplace_back(ClientEvents::EventType::Console);

		return (int)m_events.size() - 1;
	}

	void ClientEvents::RemoveEvent(const int& index)
	{
		index;
	}

	int ClientEvents::EvaluateEvent(const int& index, TCPData& data, std::list<std::string>& allMessages, const size_t& maxMessageLen)
	{
		switch (m_typeEventsBuffer[index])
		{
		case EventType::Client: return Receive(m_clientIds[index], data);  break;
		case EventType::Console: allMessages = this->ReadKeyboardInput(index, maxMessageLen);  break;
		default: break;
		}

		return EXIT_SUCCESS;
	}

	static int I = 0;
	int ClientEvents::WaitForEvents(int& index)
	{
		DWORD result = WaitForMultipleObjects((DWORD)m_events.size(), m_events.data(), FALSE, INFINITE);

		if (result == WAIT_FAILED) {
			ReportError(WSAGetLastError());
			index = -1;
			return EXIT_WaitForMultipleObj;
		}

		if (result < 0) {
			index = result + WAIT_ABANDONED_0;
			return EXIT_WaitForMultipleObj;
		}

		DebugMessage("Wait successfull: %d\n", I++);
		index = result;
		return EXIT_SUCCESS;
	}

	void ClientEvents::ClearConsoleInputs()
	{
		DWORD num;
		for (int i = 0; i < m_typeEventsBuffer.size(); i++) {
			if (m_typeEventsBuffer[i] == EventType::Console)
				ReadConsoleInput(m_events[i], &m_lpBuffer[0], ARRAYSIZE(m_lpBuffer), &num);
		}
	}

	std::list<std::string> ClientEvents::ReadKeyboardInput(const int& index, const size_t& maxLen)
	{
		DWORD num;
		std::list<std::string> messages;
		//GetNumberOfConsoleInputEvents(m_events[index], &num);
		//DebugMessage("Num Inputs: %d\n", (int)num);

		ReadConsoleInput(m_events[index], &m_lpBuffer[0], ARRAYSIZE(m_lpBuffer), &num);
		for (DWORD i = 0; i < num; i++)
		{
			switch (m_lpBuffer[i].EventType)
			{
			case KEY_EVENT: 
				if (m_lpBuffer[i].Event.KeyEvent.bKeyDown) {
					TCHAR c = ConvertUchar(m_lpBuffer[i].Event.KeyEvent.uChar);
					m_inputMessage += c;
					//ConsoleMessage(&m_inputMessage.back());

					if (c == TEXT('\r') || m_inputMessage.length() >= maxLen) {
						messages.emplace_back(std::move(m_inputMessage));
					}
				}
				break;

			default:
				break;
			}
		}

		return messages;
	}
}