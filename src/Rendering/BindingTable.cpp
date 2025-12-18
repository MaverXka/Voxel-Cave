#include "BindingTable.h"
#include <d3d12.h>
#include "Renderer.h"
#include "Debug/Log/Logger.h"

BindingTable::BindingTable()
{

}

BindingTable::~BindingTable()
{
}

BindingTable& BindingTable::Build()
{
	if (Assembled) return *this;
	auto D3DDevice = Renderer::instance->M_D3D12Device;
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = LinkedResources;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	if (FAILED(D3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&DescriptorHeap))))
	{
		Logger::Get()->Log("D3D12 descriptor heap creation failed.");
	}
	else
	{
		Logger::Get()->LogFormat("D3D12 descriptor heap creation succeed with {} descriptors", LinkedResources);
		Assembled = true;
	}

	IncrementSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return *this;
}

BindingTable& BindingTable::AddSRV()
{
	LinkedResources++;
	return *this;
}

BindingTable& BindingTable::AddCBV()
{
	LinkedResources++;
	return *this;
}

BindingTable& BindingTable::AddUAV()
{
	LinkedResources++;
	return *this;
}

BindingTable& BindingTable::Reserve(unsigned int Count)
{
	LinkedResources += Count;
	return *this;
}

D3D12_GPU_DESCRIPTOR_HANDLE BindingTable::GetGpuHandle(unsigned int Offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += static_cast<unsigned long long>(Offset)*GetIncSize();
	return handle;
}
