#include "IndexBuffer.h"
#include <d3dx12.h>
#include <format>
#include "Renderer.h"
#include "Debug/Log/Logger.h"


void IndexBuffer::Build()
{
	if (Assembled)
	{
		Logger::Get()->Log("Index buffer is already assembled.");
		return;
	}

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC indexbufferdesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	auto D3DDevice = Renderer::instance->M_D3D12Device;

	if (FAILED(D3DDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &indexbufferdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&BufferResource))))
	{
		Logger::Get()->Log("D3D12 Buffer resource creation failed.");
	}

	CD3DX12_RANGE readRange(0, 0);
	if (FAILED(BufferResource->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin))))
	{
		Logger::Get()->Log("Index buffer resource map failed.");
	}

	memcpy(pDataBegin, Data, BufferSize);
	BufferResource->Unmap(0, nullptr);

	IndexBufferView.BufferLocation = BufferResource->GetGPUVirtualAddress();
	IndexBufferView.SizeInBytes = BufferSize;
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//Logger::Get()->LogString(std::format("D3D12 index buffer stride: {}", IndexBufferView.StrideInBytes));

	Logger::Get()->LogString(std::format("D3D12 index buffer uploaded. Size: {}", BufferSize));
	Assembled = true;
}

void IndexBuffer::UploadIndices(int indicesCount, size_t StrideSize, void* NewData)
{
	IndicesCount = indicesCount;
	BufferSize = indicesCount*StrideSize;
	Data = NewData;
}
