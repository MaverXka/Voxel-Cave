#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "Buffer.h"


class IndexBuffer : public Buffer
{
public:

	virtual void Build() override;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()
	{
		return IndexBufferView;
	}

	void UploadIndices(int indicesCount, size_t StrideSize, void* Data);

	int GetIndicesCount()
	{
		return IndicesCount;
	}

private:

	int IndicesCount;

	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

};
