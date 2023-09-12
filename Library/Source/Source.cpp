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
		return sizeof(this->buffer);
	}


	ServerEvents::ServerEvents()
	{
	}

	int ServerEvents::PoolEvents()
	{
		static const int WAIT_TIME_MS = -1;

		int result = WSAPoll(m_events.data(), (ULONG)m_events.size(), (INT)WAIT_TIME_MS);
		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_WSAPool;
		}

		DebugMessage("Pool successfull\n");
		if (result == 0) {
			DebugMessage("Nothing Pooled\n");
		}

		return result;
	}

	int ServerEvents::AddServerEvent(const uintptr_t& serverId)
	{
		if (!m_events.empty())
			return EXIT_AddServerEvent;

		this->AddEvent(serverId);
		return EXIT_SUCCESS;
	}


	void ServerEvents::AddEvent(const uintptr_t& socketId)
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
				m_events[i].fd = (SOCKET) - 1; //ignore when pooling
				DebugMessage("Remove successfull");
				break;
			}
		}

		DebugMessage("No Remove");
	}

	int ServerEvents::EvaluateEvents(uintptr_t& socketId, LibNetwork::TCPData& data, int indexHint = 0)
	{
		int result;
		if (indexHint == 0 && m_events[0].revents & POLLRDNORM) { //Server socket
			if (Accept(m_events[0].fd, socketId) == EXIT_SUCCESS) {
				DebugMessage("New Client successfull");
				this->AddEvent(socketId);
			} else {
				DebugMessage("Error new Client");
			}

			return 1;
		}

		for (int i = indexHint; i < m_events.size(); i++) { //clients
			if (!(m_events[i].revents & (POLLRDNORM | POLLERR))) //TODO : deal with EvaluateEvents POLLERR
				continue;

			if ((result = Receive(m_events[i].fd, data)) != EXIT_SUCCESS) { //error so disconnect
				DebugMessage("Client disconected");
				CloseSocket(m_events[i].fd);
				this->RemoveEvent(m_events[i].fd);
			
				//TODO: if receive is to big do it twice
			}

			return i + 1;
		}

		return -1;
	}


	ClientEvents::ClientEvents()
	{}

	void ClientEvents::Run()
	{
		WaitForMultipleObjects(maxIndex + 1, FALSE, WAIT_TIME_MS);

	}

	int ClientEvents::AddEvent()
	{
		HANDLE newEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (newEvent == NULL) {
			ReportError(WSAGetLastError());
			return EXIT_AddClientEvent;
		}

		m_events.emplace(std::move(newEvent));
	}
}