#pragma once

#include <cstring>
#include <algorithm>
#include <vector>
#include <Winsock2.h>
#include <unordered_set>


namespace LibNetwork {

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
		size_t GetBufferSize();

		TCPDataType type = TCPDataType::DEFAULT;
		char buffer[99] = {};
	};


	class ServerEvents
	{
	public:
		ServerEvents();

		int AddServerEvent(const uintptr_t& serverId);
		void AddEvent(const uintptr_t& socketId);
		void RemoveEvent(const uintptr_t& socketId);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="socketId">: event's socket id [out]</param>
		/// <param name="data">: data read form socket [out]</param>
		/// <param name="indexHint">: check events starting from hint</param>
		/// <returns> 1 if accepted new client, indexHint for next function call or -1 if reached end</returns>
		int EvaluateEvents(uintptr_t& socketId, LibNetwork::TCPData& data, int indexHint);

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Number of events that have queried status, or negative error code</returns>
		int PoolEvents();

	private:
		std::vector<pollfd> m_events;
		std::unordered_set<int> m_unusedEventBuffer;
	};

	class ClientEvents
	{
	public:
		ClientEvents();

		void Run();
		int AddEvent();

	private:
		std::unordered_set<HANDLE> m_events;
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



