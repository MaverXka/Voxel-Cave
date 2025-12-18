#include "BindingTableAllocator.h"
#include <d3d12.h>
#include "Debug/Log/Logger.h"
#include "Renderer.h"

BindingTableAllocator::BindingTableAllocator()
{
	allocatedIndexList = std::vector<uint64_t>((1000+63)/64,0);
}

BindingTableAllocator::~BindingTableAllocator()
{
}

BindingHandle BindingTableAllocator::Allocate()
{
	for (size_t ba = 0; ba < allocatedIndexList.size(); ++ba)
	{
		uint64_t freeBits = ~allocatedIndexList[ba];
		if (freeBits)
		{
			unsigned long bit;
			_BitScanForward64(&bit, freeBits);
			BindingHandle handle;
			handle.Index = ba * 64 + bit;
			Set(handle.Index);
			return handle;
		}

	}
	BindingHandle handle;
	handle.Index = -1;
	return handle;
}

void BindingTableAllocator::Free(BindingHandle& Handle)
{
	if (!Handle.Valid()) return;
	allocatedIndexList[Handle.Index >> 6] &= ~(1ull << (Handle.Index & 63));
	Handle.Index = 0xFFFFFFFFu;
}

void BindingTableAllocator::Build(unsigned int ReserveCount)
{
	if (Assembled) return;
	LinkedResources = ReserveCount;
	auto D3DDevice = Renderer::instance->M_D3D12Device;
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = ReserveCount;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	if (FAILED(D3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&DescriptorHeap))))
	{
		Logger::Get()->Log("D3D12 descriptor heap creation failed.");
	}
	else
	{
		Logger::Get()->LogFormat("D3D12 descriptor heap (Allocator) creation succeed with {} descriptors", LinkedResources);
		Assembled = true;
	}

	IncrementSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return;
}

D3D12_GPU_DESCRIPTOR_HANDLE BindingTableAllocator::GetGpuHandle(BindingHandle& Handle)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += static_cast<unsigned long long>(Handle.Index) * GetIncSize();
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE BindingTableAllocator::GetCpuHandle(BindingHandle& Handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += static_cast<unsigned long long>(Handle.Index) * GetIncSize();
	return handle;
}

void BindingTableAllocator::Set(size_t Idx)
{
	Logger::Get()->LogFormat("Binding table allocator. Allocated at: {}", Idx);
	allocatedIndexList[Idx >> 6] |= 1ull << (Idx & 63);
}
