#include "Header.h"
#include "NetworkErrors.h"
#include "InputOutput.h"
#include <vector>
#include <type_traits>

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

		return FUNC_SUCCESS;
	}

	int Disconnect()
	{
		int result = WSACleanup();

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_WSACleanup;
		}

		DebugMessage("Disconnect successfull\n");
		return FUNC_SUCCESS;
	}

	int CreateSocket(uintptr_t& socketId, bool isIPV4)
	{
		SOCKET s = socket(isIPV4? AF_INET: AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (s == INVALID_SOCKET){
			ReportError(WSAGetLastError());
			return EXIT_socket;
		}
		socketId = (uintptr_t)s;

		DebugMessage("CreateSocket successfull\n");
		return FUNC_SUCCESS;
	}

	int CloseSocket(const uintptr_t& socketId)
	{
		int result = closesocket((SOCKET)socketId);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_closesocket;
		}

		DebugMessage("CloseSocket successfull\n");
		return FUNC_SUCCESS;
	}

	int BindSocket(const uintptr_t& serverSocket, bool isIPV4)
	{
		//sockaddr_in name;
		//name.sin_family = AF_INET;
		//InetPton(AF_INET, ADRESS_ANY_IPV4, &name.sin_addr.S_un.S_addr);
		//name.sin_port = htons(PORT_NUMBER);

		sockaddr* addrInfo = nullptr;
		sockaddr_in addrInfoIPV4 = {};
		sockaddr_in6 addrInfoIPV6 = {};
		if (isIPV4) { //AF_INET
			addrInfoIPV4.sin_family = AF_INET;
			InetPton(AF_INET, "0.0.0.0", &addrInfoIPV4.sin_addr.S_un.S_addr);
			addrInfoIPV4.sin_port = htons(PORT_NUMBER);
			addrInfo = (sockaddr*)&addrInfoIPV4;
		}
		else { //AF_INET6
			addrInfoIPV6.sin6_family = AF_INET6;
			InetPton(AF_INET6, "::", &addrInfoIPV6.sin6_addr);
			addrInfoIPV6.sin6_port = htons(PORT_NUMBER);
			addrInfo = (sockaddr*)&addrInfoIPV6;
		}

		int result = bind((SOCKET)serverSocket, addrInfo, isIPV4? sizeof(sockaddr_in) : sizeof(sockaddr_in6));

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_bind;
		}

		DebugMessage("BindSocket successfull\n");
		return FUNC_SUCCESS;
	}

	int Listen(const uintptr_t& serverSocket, int backlog)
	{
		int result = listen((SOCKET)serverSocket, backlog);
		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_listen;
		}

		DebugMessage("Listen successfull\n");
		return FUNC_SUCCESS;
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
		return FUNC_SUCCESS;
	}

	ADDRESS_FAMILY AdressFamilly(const std::string& address) {
		return address.find(':') != std::string::npos ? AF_INET6 : AF_INET;
	}

	int Connect(const uintptr_t& clientSocket, const std::string& address)
	{
		ADDRESS_FAMILY family = AdressFamilly(address);

		sockaddr* addrInfo;
		sockaddr_in addrInfoIPV4 = {};
		sockaddr_in6 addrInfoIPV6 = {};
		if (family == AF_INET) {
			addrInfoIPV4.sin_family = AF_INET;
			InetPton(AF_INET, TEXT(address.c_str()), &addrInfoIPV4.sin_addr.S_un.S_addr); // 127.0.0.1 is self
			addrInfoIPV4.sin_port = htons(PORT_NUMBER);
			addrInfo = (sockaddr*)&addrInfoIPV4;
		}
		else {
			addrInfoIPV6.sin6_family = AF_INET6;
			InetPton(AF_INET6, TEXT(address.c_str()), &addrInfoIPV6.sin6_addr.u); // 127.0.0.1 is self
			addrInfoIPV6.sin6_port = htons(PORT_NUMBER);
			addrInfo = (sockaddr*)&addrInfoIPV6;
		}
		
		int result = connect((SOCKET)clientSocket, addrInfo, family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));


		//sockaddr_in addrInfoIPV4;
		//addrInfoIPV4.sin_family = 
		//InetPton(AdressFamilly(address.data()), TEXT(address.c_str()), &addrInfoIPV4.sin_addr.S_un.S_addr); // 127.0.0.1 is self
		//addrInfoIPV4.sin_port = htons(PORT_NUMBER);

		//int result = connect((SOCKET)clientSocket, (sockaddr*)&addrInfoIPV4, sizeof(in6_addr));

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_listen;
		}

		DebugMessage("Connect successfull\n");
		return FUNC_SUCCESS;
	}

	int Send(const uintptr_t& socketId, void* data, int bufferSize)
	{
		int result = send(socketId, (TCHAR*)data, bufferSize, 0);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_send;
		}

		DebugMessage(TEXT("Send successfull.\n"));

		return FUNC_SUCCESS;
	}

	int Receive(const uintptr_t& socketId, void* data, int bufferSize)
	{
		int result = recv(socketId, (TCHAR*)data, bufferSize, 0);

		if (result == SOCKET_ERROR) {
			ReportError(WSAGetLastError());
			return EXIT_recv;
		}

		if (result == 0) {
			DebugMessage("Connection gracefully closed\n");
			return EXIT_connectionClosed;
		}

		DebugMessage("Receive successfull\t");
		//DebugMessage(data.buffer);
		DebugMessage("\n");

		return FUNC_SUCCESS;
	}

	void LibNetwork::TCPData::SetBitOrder(void* buffer, size_t length, bool isNetworkToHost)
	{
		if ((void*)(&this[1]) < (void*)(&((float*)buffer)[length])) {
			DebugMessage(TEXT("Try to modify BitOrder out of buffer range\n"));
			return;
		}

		if (isNetworkToHost) {
			for (size_t i = 0; i < length; i++) {
				((float*)buffer)[i] = ntohf(((unsigned int*)buffer)[i]);
			}
			return;
		}

		for (size_t i = 0; i < length; i++) {
			((unsigned int*)buffer)[i] = htonf(((float*)buffer)[i]);
		}


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

	bool ServerEvents::IsServerIndex(const int& index)
	{
		return index >= 0 && index < m_startClientIndex;
	}

	bool ServerEvents::IsClientIndex(const int& index)
	{
		return index >= m_startClientIndex && index < m_events.size();
	}

	void ServerEvents::AddServerEvent(const uintptr_t& socketId)
	{
		m_startClientIndex++;
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

				if (i < m_startClientIndex) {
					m_startClientIndex--;
				}

				return;
			}
		}

		DebugMessage("No Remove\n");
	}

	int ServerEvents::EvaluateEvents(uintptr_t& socketId, void* data, int bufferSize, int& index)
	{
		int result;
		for (index; index < m_startClientIndex; index++) { //server
			if (m_events[index].revents & POLLRDNORM) {
				socketId = m_events[index].fd;
				return (result = Accept(m_events[index].fd, socketId)) == FUNC_SUCCESS ? 1 : -1;
			}
		}

		for (index; index < m_events.size(); index++) { //clients
			if (m_events[index].revents & (POLLERR | POLLHUP | POLLRDNORM)) {
				socketId = m_events[index].fd;
				return (result = Receive(socketId, data, bufferSize)) == FUNC_SUCCESS? 0: -1; //same as return Receive
			}
		}

		index++;
		DebugMessage("End of Events\n");
		return -1;
	}

	ClientEvents::ClientEvents() : m_lpBuffer()
	{
		m_inputMessage.emplace_back(std::string());
	}

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

		WaitForEvents(index);

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

	int ClientEvents::EvaluateEvent(const int& index, void* data, int bufferSize, std::vector<std::string>& allMessages, const size_t& maxMessageLen)
	{
		switch (m_typeEventsBuffer[index])
		{
		case EventType::Client: return Receive(m_clientIds[index], data, bufferSize);  break;
		case EventType::Console: allMessages = this->ReadKeyboardInput(index, maxMessageLen);  break;
		default: break;
		}

		return FUNC_SUCCESS;
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
		return FUNC_SUCCESS;
	}

	void ClientEvents::ClearConsoleInputs()
	{
		DWORD num;
		for (int i = 0; i < m_typeEventsBuffer.size(); i++) {
			if (m_typeEventsBuffer[i] == EventType::Console)
				ReadConsoleInput(m_events[i], &m_lpBuffer[0], ARRAYSIZE(m_lpBuffer), &num);
		}
	}

	std::vector<std::string> ClientEvents::ReadKeyboardInput(const int& index, const size_t& maxLen)
	{
		DWORD num;
		m_inputMessage.back().reserve(maxLen);
		std::vector<std::string> message;

		ReadConsoleInput(m_events[index], &m_lpBuffer[0], ARRAYSIZE(m_lpBuffer), &num);
		for (DWORD i = 0; i < num; i++)
		{
			switch (m_lpBuffer[i].EventType)
			{
			case KEY_EVENT: 
				if (m_lpBuffer[i].Event.KeyEvent.bKeyDown) {
					TCHAR c[2] = { ConvertUchar(m_lpBuffer[i].Event.KeyEvent.uChar), TEXT('\0') };

					if (c[0] == TEXT('\r')){
						if (m_inputMessage[0].empty()) {
							//ConsoleMessage("\n\b");
						}
						else {
							message = std::move(m_inputMessage);
							m_inputMessage.emplace_back("");
							ConsoleMessage("\n");
						}
						continue;
					}

					m_inputMessage.back() += c[0];

					ConsoleMessage(&c[0]);
					if (m_inputMessage.back().length() >= maxLen - 1) {
						m_inputMessage.back() += '\0';
						m_inputMessage.emplace_back("");
						m_inputMessage.back().reserve(maxLen);
					}
				}
				break;

			default:
				break;
			}
		}

		return message;
	}
}