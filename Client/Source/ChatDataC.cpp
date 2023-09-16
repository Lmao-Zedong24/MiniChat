//ChatData.cpp
#include "ChatDataC.h"

ChatData& ChatData::WriteInBuffer(const char* info, size_t offset)
{
	size_t infoLen = strlen(info);
	size_t bufMax = (size_t)GetMessageBufferSize();
	size_t bufLen = bufMax - offset - 1;

	memcpy_s(&(buffer[offset]), bufLen, info, std::min<size_t>(bufLen, infoLen));
	buffer[std::min<size_t>(bufMax, infoLen + offset)] = '\0';

	return *this;
}

size_t ChatData::GetMessageBufferSize()
{
	return ARRAYSIZE(ChatData::buffer);
}

void* ChatData::PrepareData(bool isNetworkToHost)
{
	void* data = (void*)this;
	isNetworkToHost;
	//this->SetBitOrder(data, 1, isNetworkToHost); //set enum
	return data;
}

int ChatData::BufferSize()const
{
	return sizeof(ChatData);
}
