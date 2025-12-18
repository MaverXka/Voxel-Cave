#include "GraphicsCommandList.h"
#include "Renderer.h"
#include "Debug/Log/Logger.h"
#include "PSO.h"
#include <d3d12.h>



GraphicsCommandList::GraphicsCommandList()
{
	auto D3DDevice = Renderer::instance->M_D3D12Device;
	if (!D3DDevice)
	{
		Logger::Get()->Log("Cannot get a d3d device.");
		return;
	}

	ThrowIfFailed(D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())),"Failed to create command allocator.");
	ThrowIfFailed(D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())), "Failed to create graphics command list.");
	ThrowIfFailed(CommandList->Close(), "Failed to close graphics command list.");

}

GraphicsCommandList::GraphicsCommandList(ID3D12Device* Device)
{
	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())), "Failed to create command allocator.");
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())), "Failed to create graphics command list.");
	ThrowIfFailed(CommandList->Close(), "Failed to close graphics command list.");
}

GraphicsCommandList::~GraphicsCommandList()
{
}

void GraphicsCommandList::BeginRecord()
{
	ThrowIfFailed(CommandAllocator->Reset(), "Failed to reset command allocator.");
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr), "Failed to reset command allocator.");
}

void GraphicsCommandList::BeginRecord(PSO* PipelineState)
{
	ThrowIfFailed(CommandAllocator->Reset(), "Failed to reset command allocator.");
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), PipelineState->GetPSO().Get()), "Failed to reset command allocator.");
}

void GraphicsCommandList::EndRecord()
{
	ThrowIfFailed(CommandList->Close(), "Failed to close graphics command list.");
}
