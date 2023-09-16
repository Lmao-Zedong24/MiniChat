//ChatData.h
#pragma once
#include <Network/Header.h>

enum class ChatDataType {
	DEFAULT,
	MESSAGE,
	NAME,
	DISCONNECT
};


struct ChatData : public LibNetwork::TCPData
{
	ChatData& WriteInBuffer(const char* info, size_t offset = 0);
	static size_t GetMessageBufferSize();

	void* PrepareData(bool isNetworkToHost)override;
	int BufferSize() const override;

	ChatDataType type = ChatDataType::DEFAULT;
	char buffer[99] = {};
};