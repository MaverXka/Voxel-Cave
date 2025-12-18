#pragma once
#include <wrl.h>
#include <d3d12.h>


using namespace Microsoft::WRL;

class RootSignature
{
public:

	RootSignature();

	ComPtr<ID3D12RootSignature> GetRootSignature()
	{
		return M_RootSignature;
	}

protected:

	ComPtr<ID3D12RootSignature> M_RootSignature;

};
