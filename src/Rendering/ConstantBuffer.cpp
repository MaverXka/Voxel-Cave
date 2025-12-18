#include "ConstantBuffer.h"
#include <d3dx12.h>
#include <format>
#include "Renderer.h"
#include "Debug/Log/Logger.h"

#include "BindingTableAllocator.h"



ConstantBuffer::ConstantBuffer(BindingTable* InBindingTable) : LocalBindingTable(InBindingTable)
{
	if (!InBindingTable)
	{
		Logger::Get()->Log("Failed to init constant buffer. Binding Table is not referenced.");
		return;
	}
}

ConstantBuffer::~ConstantBuffer()
{
	BufferResource->Unmap(0, nullptr);
}

void ConstantBuffer::Build()
{
	if (Assembled)
	{
		Logger::Get()->Log("Constant buffer is already assembled.");
		return;
	}
	auto D3DDevice = Renderer::instance->M_D3D12Device;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferdesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);
	Logger::Get()->LogString(std::format("D3D12 constant buffer. Size: {}", BufferSize));

	if (FAILED(D3DDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&BufferResource))))
	{
		Logger::Get()->Log("D3D12 Buffer resource creation failed.");
	}

	ConstantBufferView.BufferLocation = BufferResource->GetGPUVirtualAddress();
	ConstantBufferView.SizeInBytes = BufferSize;
	BindingTableAllocator* bta = reinterpret_cast<BindingTableAllocator*>(LocalBindingTable);
	CBVHandle = bta->Allocate();
	D3DDevice->CreateConstantBufferView(&ConstantBufferView, bta->GetCpuHandle(CBVHandle));

	CD3DX12_RANGE readRange(0, 0);
	if (FAILED(BufferResource->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin))))
	{
		Logger::Get()->Log("Failed to map constant buffer.");
	}
	memcpy(pDataBegin, Data, BufferSize);

	Assembled = true;
	
}

void ConstantBuffer::UpdateConstantBufferData(void* NewData, size_t Size)
{
	//ZeroMemory(pDataBegin, BufferSize);
	memcpy(pDataBegin, NewData, Size);
}

void ConstantBuffer::UploadData(size_t Size, void* NewData)
{
	Buffer::UploadData(Size, NewData);
}
