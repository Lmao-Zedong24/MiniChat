//ChatClient.cpp
#include "ChatClient.h"
#include "Network/Header.h"
#include "Network/InputOutput.h"
#include <winnt.h>

using namespace LibNetwork;

ChatClient::ChatClient() : m_clientSocket(0), m_offset(0)
{
}

int ChatClient::OpenClient()
{
	int result = Initialize();
	if (result != EXIT_SUCCESS)
		return result;

	result = CreateSocket(m_clientSocket);
	if (result != EXIT_SUCCESS)
		return result;

	result = Connect(m_clientSocket);
	return result;
}

int ChatClient::CloseClient()
{
	int result;
	if ((result = CloseSocket(m_clientSocket)) != EXIT_SUCCESS)
		return result;

	return Disconnect();
}

void ChatClient::ReceiveData()
{
	int result;
	if ((result = Receive(m_clientSocket, m_data)) != EXIT_SUCCESS) {
		//TODO: ChatClient::ReceiveData error
		return;
	}

	switch (m_data.type)
	{
	case LibNetwork::TCPDataType::MESSAGE: ConsoleMessage(m_data.buffer); break;
	case LibNetwork::TCPDataType::NAME:  SendClientName(); break;
	case LibNetwork::TCPDataType::DISCONNECT: /*TODO: ReceiveData DISCONNECT*/ break;
	case LibNetwork::TCPDataType::DEFAULT: /*TODO: ReceiveData DEFAULT*/
	default: break;
	}

}

int ChatClient::SendClientName()
{
	m_data.type = LibNetwork::TCPDataType::NAME;

	while (m_name.empty() || m_name.length() > MAX_NAME_LENGTH) {
		ConsoleMessage(TEXT("Enter name (10 char. max): ")); //TODO : upgrade ConsoleMessage()
		m_name = GetUserInput();
	}

	m_data.WriteInBuffer(m_name.c_str());
	int result = Send(m_clientSocket, m_data);

	m_data.WriteInBuffer(MESSAGE_SEPARATOR.c_str(), m_name.length());
	m_offset = m_name.length() + MESSAGE_SEPARATOR.length();

	return result;
}

int ChatClient::SendClientMessage()
{
	m_data.type = LibNetwork::TCPDataType::MESSAGE;
	m_data.WriteInBuffer(GetUserInput().c_str(), m_offset);
	return Send(m_clientSocket, m_data);
}
