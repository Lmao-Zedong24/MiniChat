//ChatClient.cpp
#include "ChatClient.h"
#include "Network/Header.h"
#include "Network/InputOutput.h"
#include <winnt.h>

using namespace LibNetwork;

ChatClient::ChatClient() : m_clientSocket(), m_offset()
{
}

int ChatClient::OpenClient()
{
	int result;
	if ((result = Initialize()) != EXIT_SUCCESS)
		return result;

	if ((result = CreateSocket(m_clientSocket)) != EXIT_SUCCESS)
		return result;

	if ((result = Connect(m_clientSocket)) != EXIT_SUCCESS)
		return result;

	if ((result = m_events.AddClientEvent(m_clientSocket)) < 0)
		return result;

	if ((result = m_events.AddConsoleEvent()) < 0)
		return result;

	return EXIT_SUCCESS;
}

int ChatClient::CloseClient()
{
	int result;
	if ((result = CloseSocket(m_clientSocket)) != EXIT_SUCCESS)
		return result;

	return Disconnect();
}

void ChatClient::Run()
{
	while (true) {
		int index;
		int result = m_events.WaitForEvents(index);
		m_data.type = TCPDataType::DEFAULT;

		if (result != EXIT_SUCCESS) {
			if (index == -1) { //ERROR in wait 
				DebugMessage("ERROR in wait, return");
				return;
			}
			else { //Aborted index
				DebugMessage("Aborted event, return");
				m_events.RemoveEvent(index);
				return;
			}
		}

		std::vector<std::string> clientMessages;
		if ((result = m_events.EvaluateEvent(index, m_data, clientMessages, TCPData::GetBufferSize() - m_offset)) != EXIT_SUCCESS) {
			//TODO: ChatClient::EvaluateEvent error
			DebugMessage("ChatClient::EvaluateEvent error");
			//return;
		}

		if (!clientMessages.empty()) {
			ConsoleMessage((m_name + MESSAGE_SEPARATOR).c_str());
			for (auto& message: clientMessages)
				this->SendClientMessage(message);

			continue;
		}

		switch (m_data.type)
		{
		case TCPDataType::MESSAGE:	ConsoleMessage(m_data.buffer); break;
		case TCPDataType::NAME:		SendClientName(); break;
		case TCPDataType::DEFAULT:	
		default: break;
		}
		
	}
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

	m_events.ClearConsoleInputs();
	ConsoleMessage(m_data.buffer);
	return result;
}

int ChatClient::SendClientMessage(const std::string& message)
{
	m_data.type = LibNetwork::TCPDataType::MESSAGE;
	m_data.WriteInBuffer((m_name + MESSAGE_SEPARATOR + message).c_str());
	return Send(m_clientSocket, m_data);
}

//int TestConsoleInput();
