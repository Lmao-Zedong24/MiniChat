#pragma once

#include <cstring>
#include <algorithm>
#include <vector>
#include <Winsock2.h>
#include <unordered_set>
#include <unordered_map>
#include <string>


namespace LibNetwork {

#ifdef UNICODE
	#define ConvertUchar(c) c.UnicodeChar
#else
	#define ConvertUchar(c) c.AsciiChar
#endif

	constexpr int PORT = 43210; //TODO : Put better port number

	enum class TCPDataType {
		DEFAULT,
		MESSAGE,
		NAME,
		DISCONNECT
	};

	struct TCPData
	{
		TCPData& WriteInBuffer(const char* info, size_t offset = 0);
		static size_t GetBufferSize();

		TCPDataType type = TCPDataType::DEFAULT;
		char buffer[99] = {};
	};


	class ServerEvents
	{
	public:
		ServerEvents();

		int AddServerEvent(const uintptr_t& serverId);
		void AddClientEvent(const uintptr_t& socketId);
		void RemoveEvent(const uintptr_t& socketId);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="socketId">: new clients event's socket id [out]</param>
		/// <param name="data">: data read form socket [out]</param>
		/// <param name="index">: check events starting from hint [in/out]</param>
		/// <returns> 0 if receive from client, 1 if accept, or negative error code</returns>
		int EvaluateEvents(uintptr_t& socketId, LibNetwork::TCPData& data, int& index);

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Number of events that have queried status, or negative error code</returns>
		int WaitForEvents();

		const uintptr_t& GetSocketId(const int& index);

	private:
		std::vector<pollfd> m_events;
		std::unordered_set<int> m_unusedEventBuffer;
	};

	class ClientEvents
	{
	public:
		ClientEvents();

		enum class EventType;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="socketId"></param>
		/// <returns>Event's index, or negative error code</returns>
		int AddClientEvent(const uintptr_t& socketId);
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Event's index, or negative error code</returns>
		int AddConsoleEvent();
		void RemoveEvent(const int& index);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="index">: [in]</param>
		/// <param name="data">: [out]</param>
		/// <param name="consoleInput">: [out]</param>
		/// <returns></returns>
		int EvaluateEvent(const int& index, TCPData& data, std::vector<std::string>& consoleInput, const size_t& maxMessageLen);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="index">: the signaled handle's index or -1 if none signaled</param>
		/// <returns>EXIT_SUCCESS or negative error code</returns>
		int WaitForEvents(int& index);

		void ClearConsoleInputs();

		const std::vector<HANDLE>& Events() { return m_events; }

	private:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="index">: [in]</param>
		/// <returns>consoleInput</returns>
		std::vector<std::string> ReadKeyboardInput(const int& index, const size_t& maxLen = 0);


		std::vector<HANDLE> m_events;
		std::vector<EventType> m_typeEventsBuffer;
		std::unordered_map<int, uintptr_t> m_clientIds;
		INPUT_RECORD m_lpBuffer[99];
		std::vector<std::string> m_inputMessage;
	};

	int Initialize();
	int Disconnect();

	/// <summary>
	/// Gets a socket port a with socket()
	/// </summary>
	/// <param name="serverSocket"> avaible socket port [out]</param>
	/// <returns> EXIT_SUCCESS or EXIT_socket if there's an error </returns>
	int CreateSocket(uintptr_t& serverSocket);
	int CloseSocket(const uintptr_t& serverSocket);
	int BindSocket(const uintptr_t& serverSocket);
	int Listen(const uintptr_t& serverSocket, int backlog);
	/// <summary>
	/// Accepts connection with client
	/// </summary>
	/// <param name="serverSocket"> server's socket [in]</param>
	/// <param name="clientSocket"> client's socket created on the server side [out]</param>
	/// <returns></returns>
	int Accept(const uintptr_t& serverSocket, uintptr_t& clientSocket);
	int Connect(const uintptr_t& serverSocket);
	int Send(const uintptr_t& socketId, const TCPData& data);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="socketId"></param>
	/// <param name="messageReceive"> messaged recieved [out]</param>
	/// <returns></returns>
	int Receive(const uintptr_t& socketId, const TCPData& data);

}



