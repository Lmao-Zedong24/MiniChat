//ChatData.cpp
#include "ChatData.h"

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

	this->SetBitOrder(data, 1, isNetworkToHost); //set enum
	return data;
}
