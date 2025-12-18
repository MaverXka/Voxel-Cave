#pragma once

#include <wrl.h>

using namespace Microsoft::WRL;

struct ID3D12GraphicsCommandList;
struct ID3D12Device;
class PSO;

class GraphicsCommandList
{
public:

	GraphicsCommandList(ID3D12Device* Device);
	GraphicsCommandList();
	~GraphicsCommandList();

	void BeginRecord();
	void BeginRecord(PSO* PipelineState);
	void EndRecord();

	ID3D12GraphicsCommandList* operator->()
	{
		return CommandList.Get();
	};

	ID3D12GraphicsCommandList* Get()
	{
		return CommandList.Get();
	}

protected:

	ComPtr<struct ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

};