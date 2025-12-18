#pragma once

#include "BindingTable.h"
#include <vector>

struct BindingHandle
{
private:



	friend class BindingTableAllocator;

public:

	unsigned int Index = 0xFFFFFFFFu;

	bool Valid() const {return Index != 0xFFFFFFFFu; }
};

struct D3D12_CPU_DESCRIPTOR_HANDLE;

class BindingTableAllocator : protected BindingTable
{
public:
	BindingTableAllocator();
	~BindingTableAllocator();

	BindingHandle Allocate();
	void Free(BindingHandle& Handle);

	void Build(unsigned int ReserveCount);

	using BindingTable::GetDescriptorHeap;
	using BindingTable::GetIncSize;
	//using BindingTable::DescriptorHeap;

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(BindingHandle& Handle);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(BindingHandle& Handle);

	BindingTable& Table() { return static_cast<BindingTable&>(*this); }

private:

	void Set(size_t Idx);

	std::vector<uint64_t> allocatedIndexList;

};
