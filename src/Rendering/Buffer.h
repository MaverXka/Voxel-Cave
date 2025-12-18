#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "Core/CoreTypes.h"



using namespace Microsoft::WRL;

class Buffer
{
public:

	virtual void UploadData(size_t Size, void* NewData);

	virtual void Build();

protected:

	void* Data;

	size_t BufferSize = 0;
	uint8* pDataBegin;

	ComPtr<ID3D12Resource> BufferResource;

	bool Assembled = false;

};
