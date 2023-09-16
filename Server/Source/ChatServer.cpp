//InterpretData.cpp Server
#include <Network/Header.h>
#include <Network/InputOutput.h>
#include <algorithm>
#include <string>
#include "ChatServer.h"

using namespace LibNetwork;

ChatServer::ChatServer() 
{
}


int ChatServer::OpenServer()
{
	int result = 0;
	if ((result = Initialize()) != FUNC_SUCCESS)
		return result;

	for (int i = 0; i < ARRAYSIZE(m_serverSockets); i++) {
		if ((result = CreateSocket(m_serverSockets[i], i == 0)) != FUNC_SUCCESS)
			return result;

		if ((result = BindSocket(m_serverSockets[i], i == 0)) != FUNC_SUCCESS)
			return result;

		if ((result = Listen(m_serverSockets[i])) != FUNC_SUCCESS)
			return result;

		m_events.AddServerEvent(m_serverSockets[i]);
	}


	return FUNC_SUCCESS;
}

void ChatServer::CloseServer()
{
	int result = 0;
	for (auto& skt : this->m_clientSockets) {
		CloseSocket(skt);
	}

	for (auto& socket : m_serverSockets) {
		CloseSocket(socket);
	}

	result = Disconnect();
}

void ChatServer::Run()
{
	ChatData data;

	while (true) { //main loop
		int numEvents = m_events.WaitForEvents();
		int eventIndex = 0;
		uintptr_t socketId = (uintptr_t)-1;
		data.type = ChatDataType::DEFAULT;

		while(numEvents-- > 0) {
			int result = m_events.EvaluateEvents(socketId, (void*)&data, sizeof(ChatData), eventIndex); //accept and receive here

			if (result < 0) {
				DebugMessage("Ya donne Error EvaluateEvents\n");
				if (m_events.IsClientIndex(eventIndex)) //if not accept error, remove client
					this->RemoveClient(socketId);
			}
			else if (result == 1) { //accepted so needs name
				this->AddClient(socketId);

				data.type = ChatDataType::NAME;
				data.WriteInBuffer("");
				if (Send(socketId, (void*)&data, sizeof(ChatData)) != FUNC_SUCCESS)
					this->RemoveClient(socketId);
			}
			else if (result == 0){ //received
				switch (data.type)
				{
				case ChatDataType::MESSAGE: DebugMessage("Got MESSAGE\n");	this->SendDataToClients(data, socketId); break; //rellay message to other clients
				case ChatDataType::NAME: DebugMessage("Got NAME\n"); this->JoinNewClient(data, socketId);	 break;
				case ChatDataType::DISCONNECT: //TODO: disconnect client
				case ChatDataType::DEFAULT:	//TODO: default client
				default:
					break;
				}
			}

			eventIndex++; //so that next evaluate only checks after current events 
		}
	}
}

void ChatServer::SendDataToClients(	ChatData& data,
									const uintptr_t& ignoreClient)
{
	int result = 0;
	std::unordered_map<int, int> errorSocketBuffer; // (index, error code)

	for (int i = 0; i < this->m_clientSockets.size(); i++) {
		if (ignoreClient == m_clientSockets[i] || !m_clientNames.contains(m_clientSockets[i])) { //ignore or dont have name
			continue;
		}
		if ((result = Send(this->m_clientSockets[i], (void*)&data, sizeof(ChatData))) != FUNC_SUCCESS) {
			errorSocketBuffer.emplace(i, result);
		}
	}

	//TODO: deal with send errors
	if (!errorSocketBuffer.empty()) {
		DebugMessage(TEXT("YOu Fucked the send to all"));
	}
}

void ChatServer::SendDataToClients(	ChatData& data,
									const std::unordered_set<uintptr_t>& ignoreClients)
{
	int result = 0;
	std::unordered_map<int, int> errorSocketBuffer; // (index, error code)

	for (int i = 0; i < this->m_clientSockets.size(); i++) {
		if (ignoreClients.contains(m_clientSockets[i]) || m_clientNames.contains(m_clientSockets[i])) { //ignore or dont have name
			continue;
		}
		if ((result = Send(this->m_clientSockets[i], (void*)&data, sizeof(ChatData))) != FUNC_SUCCESS) {
			errorSocketBuffer.emplace(i, result);
		}
	}

	//TODO: deal with send errors
}

void ChatServer::AddClient(const uintptr_t& socketId)
{
	m_clientSockets.push_back(socketId);
	m_events.AddClientEvent(socketId);
}

void ChatServer::RemoveClient(const uintptr_t& socketId)
{
	m_events.RemoveEvent(socketId);
	m_clientNames.erase(socketId);

	for (auto itt = m_clientSockets.begin(); itt != m_clientSockets.end(); itt++) {
		if (*itt == socketId) {
			m_clientSockets.erase(itt);
			break;
		}
	}
}

void ChatServer::JoinNewClient(ChatData& data, const uintptr_t& newClientId)
{
	data.type = ChatDataType::MESSAGE;
	data.WriteInBuffer(NEW_CLIENT_MESSAGE, (m_clientNames.emplace(newClientId, data.buffer).first)->second.length());
	this->SendDataToClients(data, newClientId);


	if (m_clientNames.size() == 1) {
		data.WriteInBuffer(ONLY_CLIENTS_MESSAGE);
		Send(newClientId, (void*)&data, sizeof(ChatData));
		return;
	}


	data.WriteInBuffer(CURRENT_CLIENTS_MESSAGE);
	Send(newClientId, (void*)&data, sizeof(ChatData));

	for (auto& name : m_clientNames) {
		if (name.first != newClientId) {
			data.WriteInBuffer(name.second.c_str());
			Send(newClientId, (void*)&data, sizeof(ChatData));
		}
	}
}

//void ChatServer::InterpretReceive(	int result,
//									int index, 
//									const ChatData& data, 
//									std::unordered_set<int>& closeSocketBuffer)
//{
//	if (result != FUNC_SUCCESS) { 
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