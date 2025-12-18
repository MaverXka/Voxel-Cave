#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "BindingTableAllocator.h"
#include "Buffer.h"

using namespace Microsoft::WRL;



class ConstantBuffer : public Buffer
{
public:

	ConstantBuffer(BindingTable* InBindingTable);
	~ConstantBuffer();

	D3D12_CONSTANT_BUFFER_VIEW_DESC GetBufferView()
	{
		return ConstantBufferView;
	}

	virtual void Build() override;

	void UpdateConstantBufferData(void* NewData, size_t Size);

	virtual void UploadData(size_t Size, void* NewData) override;

	BindingHandle CBVHandle;

protected:

	BindingTable* LocalBindingTable;


	D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferView;

};