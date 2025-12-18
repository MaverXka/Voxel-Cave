#pragma once
#include <wrl.h>
#include <memory>
#include "BindingTableAllocator.h"

using namespace Microsoft::WRL;

class PSO;
class RootSignature;
class Shader;
class Texture;
class GraphicsCommandList;
struct CD3DX12_CPU_DESCRIPTOR_HANDLE;
class BindingTable;
class BindingTableAllocator;
struct BindingHandle;

class ChunkRenderer
{
public:
	ChunkRenderer();
	~ChunkRenderer();

	void RenderThread_PopulateCommandList(CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

	std::shared_ptr<PSO> ChunkOpaquePSO;
	std::shared_ptr<RootSignature> ChunkRootSignature;
	GraphicsCommandList* ChunkOpaqueRenderCommandList;

private:


	BindingTableAllocator* ChunkBindingTable;
	BindingHandle ChunkVoxelAtlasTextureHandle;

	std::shared_ptr<Shader> ChunkOpaqueVSShader;
	std::shared_ptr<Shader> ChunkOpaquePSShader;

	std::shared_ptr<Texture> ChunkVoxelAtlasTexture;


};