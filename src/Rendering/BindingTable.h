#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;

struct ID3D12DescriptorHeap;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

class BindingTable
{
public:
	BindingTable();
	~BindingTable();

	BindingTable& Build();

	BindingTable& AddSRV();
	BindingTable& AddCBV();
	BindingTable& AddUAV();
	BindingTable& Reserve(unsigned int Count);

	ID3D12DescriptorHeap* GetDescriptorHeap()
	{
		return DescriptorHeap.Get();
	}

	unsigned int GetIncSize()
	{
		return IncrementSize;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(unsigned int Offset);

private:

	friend class BindingTableAllocator;

	bool Assembled = false;

	unsigned int IncrementSize = 0;

	unsigned int LinkedResources = 0;

	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

};
