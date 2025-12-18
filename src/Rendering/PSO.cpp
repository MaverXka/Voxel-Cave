#include "PSO.h"
#include <d3dx12.h>
#include "Debug/Log/Logger.h"
#include "World/Chunk.h"
#include "Renderer.h"
#include "Shader.h"
#include "RootSignature.h"
#include "VertexBuffer.h"

D3D12_INPUT_ELEMENT_DESC* GetElementDesc(VertexFormat vertexFormat)
{
	switch (vertexFormat)
	{
	case PositionColor:
		Logger::Get()->Log("Accessed not implemented property.");
		return nullptr;
	case PositionUVColor:
		return const_cast<D3D12_INPUT_ELEMENT_DESC*>(PositionUVColorElementDesc);
	case PositionUV:
		Logger::Get()->Log("Accessed not implemented property.");
		return nullptr;
	default:
		return nullptr;
	}
};

unsigned int GetElementDescCount(VertexFormat vertexFormat)
{
	switch (vertexFormat)
	{
	case PositionColor:
		return 0;
	case PositionUVColor:
		return 3;
	case PositionUV:
		return 0;
	default:
		return 0;
	}
};

PSO::PSO(Shader* VertexShader, Shader* PixelShader, RootSignature* NewRootSignature)
{
	auto Device = Renderer::instance->M_D3D12Device;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = NewRootSignature->GetRootSignature().Get();
	psoDesc.InputLayout = { GetElementDesc(VertexFormat::PositionUVColor), GetElementDescCount(VertexFormat::PositionUVColor) };
	psoDesc.VS.pShaderBytecode = VertexShader->GetShaderPointer();
	psoDesc.VS.BytecodeLength = VertexShader->GetShaderBufferSize();
	psoDesc.PS.pShaderBytecode = PixelShader->GetShaderPointer();
	psoDesc.PS.BytecodeLength = PixelShader->GetShaderBufferSize();
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, FALSE, D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (SUCCEEDED(Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PipelineStateObject))))
	{
		Logger::Get()->Log("Pipeline state object created.");
	}
	else
	{
		Logger::Get()->Log("Pipeline state object creation failed.");
	}

	
}

