//InterpretData.cpp Server
#include <Network/Header.h>
#include <Network/InputOutput.h>
#include <algorithm>
#include <string>
#include "ChatServer.h"

using namespace LibNetwork;

ChatServer::ChatServer() : m_serverSocket(0)
{}

int ChatServer::OpenServer()
{
	int result = 0;
	if ((result = Initialize()) != EXIT_SUCCESS)
		return result;

	if ((result = CreateSocket(m_serverSocket)) != EXIT_SUCCESS)
		return result;

	if ((result = BindSocket(m_serverSocket)) != EXIT_SUCCESS)
		return result;

	if ((result = Listen(m_serverSocket, NUM_CLIENTS)) != EXIT_SUCCESS)
		return result;

	m_events.AddEvent(m_serverSocket);

	return EXIT_SUCCESS;
}

int ChatServer::CloseServer()
{
	int result = 0;
	for (auto& skt : this->m_clientSockets) {
		if ((result = CloseSocket(skt)) != EXIT_SUCCESS)
			return result;
	}

	if ((result = CloseSocket(this->m_serverSocket)) != EXIT_SUCCESS)
		return result;

	if ((result = Disconnect()) != EXIT_SUCCESS)
		return result;

	return EXIT_SUCCESS;
}

int ChatServer::Run()
{
	uintptr_t socketId;
	LibNetwork::TCPData data;

	while (true) { //main loop
		int numEvents = m_events.PoolEvents();
		int hint = 0;
		while(numEvents-- > 0) {
			hint = m_events.EvaluateEvents(socketId, data, hint); //accept and receive here

			if (hint == -1) {
				DebugMessage("Ya donne f*cked EvaluateEvents");
				numEvents = 0;
			}
			else if (hint == 1) { //accepted so needs name
				this->m_clientSockets.push_back(socketId);
				data.type = TCPDataType::NAME;
				data.WriteInBuffer("");
				if (Send(socketId, data) != EXIT_SUCCESS)
					DebugMessage(TEXT("YOu Fucked the send"));
			}
			else { //received
				switch (data.type)
				{
				case TCPDataType::MESSAGE: DebugMessage("Got MESSAGE\n");	this->SendDataToClients(data, socketId); break; //rellay message to other clients
				case TCPDataType::NAME: DebugMessage("Got NAME\n");	this->SendDataToClients(data.WriteInBuffer(NEW_CLIENT_MESSAGE, //add to message and rellay to other clients
																		  (	(m_clientNames.emplace(socketId, data.buffer).first)->second.length())),
																	socketId); break;
				case TCPDataType::DISCONNECT: //TODO: disconnect client
				case TCPDataType::DEFAULT:	//TODO: default client
				default:
					break;
				}
			}
		}
	}
}

void ChatServer::SendDataToClients(	const LibNetwork::TCPData& data,
									const uintptr_t& ignoreClient)
{
	int result = 0;
	std::unordered_map<int, int> errorSocketBuffer; // (index, error code)

	for (int i = 0; i < this->m_clientSockets.size(); i++) {
		if (ignoreClient == m_clientSockets[i] || !m_clientNames.contains(m_clientSockets[i])) { //ignore or dont have name
			continue;
		}
		if ((result = Send(this->m_clientSockets[i], data)) != EXIT_SUCCESS) {
			errorSocketBuffer.emplace(i, result);
		}
	}

	//TODO: deal with send errors
	if (!errorSocketBuffer.empty())
		DebugMessage(TEXT("YOu Fucked the send to all"));
}

void ChatServer::SendDataToClients(	const LibNetwork::TCPData& data,
									const std::unordered_set<uintptr_t>& ignoreClients)
{
	int result = 0;
	std::unordered_map<int, int> errorSocketBuffer; // (index, error code)

	for (int i = 0; i < this->m_clientSockets.size(); i++) {
		if (ignoreClients.contains(m_clientSockets[i]) || m_clientNames.contains(m_clientSockets[i])) { //ignore or dont have name
			continue;
		}
		if ((result = Send(this->m_clientSockets[i], data)) != EXIT_SUCCESS) {
			errorSocketBuffer.emplace(i, result);
		}
	}

	//TODO: deal with send errors
}

//void ChatServer::InterpretReceive(	int result,
//									int index, 
//									const TCPData& data, 
//									std::unordered_set<int>& closeSocketBuffer)
//{
//	if (result != EXIT_SUCCESS) { 
//		ReportError(result); //TODO : remove report error here
//		closeSocketBuffer.insert(index);
//		return;
//	}
//
//	switch (data.type) { //evalute receive
//	case MESSAGE:		this->SendDataToClients(data, m_clientSockets[index]);  break;
//	case NAME:			m_clientNames.emplace(m_clientSockets[index], data.buffer); break;
//	case DISCONNECT:	closeSocketBuffer.emplace(index); break;
//	case DEFAULT:
//	default:			break;
//	}
//}