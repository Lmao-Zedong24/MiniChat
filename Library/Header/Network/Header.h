#pragma once

#include <cstring>
#include <algorithm>
#include <vector>
#include <Winsock2.h>
#include <unordered_set>
#include <unordered_map>
#include <string>

#define FUNC_SUCCESS 0

namespace LibNetwork {

#ifdef UNICODE
	#define ConvertUchar(c) c.UnicodeChar
#else
	#define ConvertUchar(c) c.AsciiChar
#endif

	constexpr int PORT_NUMBER = 43210; //TODO : Put better port number

	class TCPData 
	{
	public:
		inline virtual void* PrepareData(bool isNetworkToHost) { isNetworkToHost; return nullptr; }
		inline virtual int BufferSize() const { return 0; }

	protected:
		void SetBitOrder(void* buffer, size_t length, bool isNetworkToHost);

	};

	class ServerEvents
	{
	public:
		ServerEvents();

		void AddServerEvent(const uintptr_t& serverId);
		void AddClientEvent(const uintptr_t& socketId);
		void RemoveEvent(const uintptr_t& socketId);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="socketId">: new clients event's socket id [out]</param>
		/// <param name="data">: data read form socket [out]</param>
		/// <param name="index">: check events starting at index, returns triggered event index [in/out]</param>
		/// <returns> 0 if receive from client, 1 if accept, or negative error code</returns>
		int EvaluateEvents(uintptr_t& socketId, void* data, int bufferSize, int& index);

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Number of events that have queried status, or negative error code</returns>
		int WaitForEvents();

		bool IsServerIndex(const int& index);
		bool IsClientIndex(const int& index);


	private:
		std::vector<pollfd> m_events;
		int m_startClientIndex = 0;
		std::unordered_set<int> m_unusedEventBuffer;
	};

	class ClientEvents
	{
	public:
		ClientEvents();

		enum class EventType {
			Client,
			Console
		};
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
		/// <summary>
		/// 
		/// </summary>
		/// <param name="index">: [in]</param>
		/// <param name="data">: [out]</param>
		/// <param name="consoleInput">: [out]</param>
		/// <returns></returns>
		int EvaluateEvent(const int& index, void* data, int bufferSize, std::vector<std::string>& consoleInput, const size_t& maxMessageLen);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="index">: the signaled handle's index or -1 if none signaled</param>
		/// <returns>FUNC_SUCCESS or negative error code</returns>
		int WaitForEvents(int& index);

		void ClearConsoleInputs();

		const std::vector<HANDLE>& GetEvents() { return m_events; }
		const std::vector<std::string>& GetInputMessage() { return m_inputMessage; }

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
	/// <returns> FUNC_SUCCESS or EXIT_socket if there's an error </returns>
	int CreateSocket(uintptr_t& serverSocket, bool isIPV4);
	int CloseSocket(const uintptr_t& serverSocket);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="serverSocket"></param>
	/// <param name="family"> true is IPV4, false is IPV6</param>
	/// <returns></returns>
	int BindSocket(const uintptr_t& serverSocket, bool isIPV4);
	int Listen(const uintptr_t& serverSocket, int backlog = SOMAXCONN);
	/// <summary>
	/// Accepts connection with client
	/// </summary>
	/// <param name="serverSocket"> server's socket [in]</param>
	/// <param name="clientSocket"> client's socket created on the server side [out]</param>
	/// <returns></returns>
	int Accept(const uintptr_t& serverSocket, uintptr_t& clientSocket);
	ADDRESS_FAMILY AdressFamilly(const std::string& address) ;
	int Connect(const uintptr_t& serverSocket, const std::string& address);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="socketId"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	int Send(const uintptr_t& socketId, void* data, int bufferSize);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="socketId"></param>
	/// <param name="messageReceive"> messaged recieved [out]</param>
	/// <returns></returns>
	int Receive(const uintptr_t& socketId, void* data, int bufferSize);
}



