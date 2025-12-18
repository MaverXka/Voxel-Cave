#pragma once

#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class Shader;
class RootSignature;

class PSO
{
public:

	PSO(Shader* VertexShader, Shader* PixelShader, RootSignature* NewRootSignature);

	ComPtr<ID3D12PipelineState> GetPSO()
	{
		return PipelineStateObject;
	}

protected:

	ComPtr<ID3D12PipelineState> PipelineStateObject;

};
