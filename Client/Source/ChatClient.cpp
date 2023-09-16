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
	if ((result = Initialize()) != FUNC_SUCCESS)
		return result;


	while (true) {
		ConsoleMessage(TEXT("Enter IP address or (q) to quit: "));
		std::string adress = GetUserInput();
		if (adress == "q") {
			return -1;
		}

		if ((result = CreateSocket(m_clientSocket, CheckFamily(adress))) != FUNC_SUCCESS)
			return result;

		if ((result = Connect(m_clientSocket, adress.c_str())) != FUNC_SUCCESS) {
			ConsoleMessage(TEXT("Couldn't connect to server\n"));
		}
		else {
			break;
		}
	}

	if ((result = m_events.AddClientEvent(m_clientSocket)) < 0)
		return result;

	if ((result = m_events.AddConsoleEvent()) < 0)
		return result;

	m_events.ClearConsoleInputs();


	return FUNC_SUCCESS;
}

int ChatClient::CloseClient()
{
	int result;
	if ((result = CloseSocket(m_clientSocket)) != FUNC_SUCCESS)
		return result;

	return Disconnect();
}

void ChatClient::Run()
{
	while (true) {
		int index;
		int result = m_events.WaitForEvents(index);
		m_data.type = ChatDataType::DEFAULT;

		if (result != FUNC_SUCCESS) {
			if (index == -1) { //ERROR in wait 
				DebugMessage("ERROR in wait, return");
				return;
			}
			else { //Aborted index
				DebugMessage("Aborted event, return");
				return;
			}
		}

		std::vector<std::string> clientMessages;
		if ((result = m_events.EvaluateEvent(index, (void*)& m_data, sizeof(ChatData),
											clientMessages, ChatData::GetMessageBufferSize() - m_offset)) != FUNC_SUCCESS) {
			//TODO: ChatClient::EvaluateEvent error
			DebugMessage("ChatClient::EvaluateEvent error");
			//return;
		}

		if (!clientMessages.empty()) {
			SaveCursor();
			ConsoleMessage((m_name + MESSAGE_SEPARATOR).c_str());
			for (auto& message: clientMessages)
				this->SendClientMessage(message);

			continue;
		}

		switch (m_data.type)
		{
		case ChatDataType::MESSAGE:	
			RestoreCursor();
			EraseFoward();
			ConsoleMessage(m_data.buffer);
			LineFeed();
			SaveCursor();
			ConsoleMessage((m_name + MESSAGE_SEPARATOR).c_str());
			for (auto& message: m_events.GetInputMessage())
				ConsoleMessage(message.c_str());
			break;
		case ChatDataType::NAME:		SendClientName(); break;
		case ChatDataType::DEFAULT:
		default: break;
		}
		
	}
}

void ChatClient::ReceiveData()
{
	int result;
	if ((result = Receive(m_clientSocket, (void*) & m_data, m_data.BufferSize())) != FUNC_SUCCESS) {
		//TODO: ChatClient::ReceiveData error
		return;
	}

	switch (m_data.type)
	{
	case ChatDataType::MESSAGE: ConsoleMessage(m_data.buffer); break;
	case ChatDataType::NAME:  SendClientName(); break;
	case ChatDataType::DISCONNECT: /*TODO: ReceiveData DISCONNECT*/ break;
	case ChatDataType::DEFAULT: /*TODO: ReceiveData DEFAULT*/
	default: break;
	}

}

int ChatClient::SendClientName()
{
	m_data.type = ChatDataType::NAME;

	while (m_name.empty() || m_name.length() > MAX_NAME_LENGTH) {
		ConsoleMessage(TEXT("Enter name (10 char. max): ")); //TODO : upgrade ConsoleMessage()
		m_name = GetUserInput();
	}

	m_data.WriteInBuffer(m_name.c_str());
	int result = Send(m_clientSocket, (void*)&m_data, sizeof(ChatData));

	m_data.WriteInBuffer(MESSAGE_SEPARATOR.c_str(), m_name.length());
	m_offset = m_name.length() + MESSAGE_SEPARATOR.length();

	m_events.ClearConsoleInputs();
	SaveCursor();
	ConsoleMessage(m_data.buffer);
	return result;
}

int ChatClient::SendClientMessage(const std::string& message)
{
	m_data.type = ChatDataType::MESSAGE;
	m_data.WriteInBuffer((m_name + MESSAGE_SEPARATOR + message).c_str());
	return Send(m_clientSocket, (void*) & m_data, sizeof(ChatData));
}

bool ChatClient::CheckFamily(const std::string& message)
{
	return AdressFamilly(message.c_str()) == AF_INET;
}

//int TestConsoleInput();
