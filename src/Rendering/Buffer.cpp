#include "Buffer.h"
#include "Debug/Log/Logger.h"

void Buffer::UploadData(size_t Size, void* NewData)
{
	Data = NewData;
	BufferSize = Size;
}

void Buffer::Build()
{
	Logger::Get()->Log("Not implemented.");
}