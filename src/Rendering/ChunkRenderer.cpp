#include "ChunkRenderer.h"
#include "RootSignature.h"
#include "Shader.h"
#include "PSO.h"
#include "Resources/ResourceManager.h"
#include "Texture.h"
#include "Debug/Log/Logger.h"
#include "GraphicsCommandList.h"
#include "World/World.h"
#include "Renderer.h"
#include "Camera.h"
#include "ConstantBuffer.h"

ChunkRenderer::ChunkRenderer()
{
	auto D3DDevice = Renderer::instance->M_D3D12Device;

	ChunkOpaqueVSShader = std::make_shared<Shader>(L"Assets/Shaders/test.hlsl", "Main", ShaderType::VS5_0);
	ChunkOpaquePSShader = std::make_shared<Shader>(L"Assets/Shaders/testps.hlsl", "Main", ShaderType::PS5_0);

	ChunkRootSignature = std::make_shared<RootSignature>();

	ChunkOpaquePSO = std::make_shared<PSO>(ChunkOpaqueVSShader.get(), ChunkOpaquePSShader.get(), ChunkRootSignature.get());

	ChunkVoxelAtlasTexture = std::shared_ptr<Texture>(reinterpret_cast<Texture*>(ResourceManager::GetResource("TEX_VoxelAtlas").get()));
	if (ChunkVoxelAtlasTexture)
	{
		Logger::Get()->Log("Chunk voxel atlas texture loaded.");
	}

	ChunkOpaqueRenderCommandList = new GraphicsCommandList();

	ChunkBindingTable = new BindingTableAllocator();
	ChunkBindingTable->Build(4096);

	ChunkVoxelAtlasTextureHandle = ChunkVoxelAtlasTexture->CreateShaderResourceView(ChunkBindingTable);

	World* world = Renderer::instance->GetWorld();
	if (!world) return;

	std::vector<Chunk*> chunks = world->GetWorldChunks();
	for (int i = 0; i < chunks.size(); i++)
	{
		chunks[i]->MakeConstantBuffer(ChunkBindingTable);
	}
	
}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::RenderThread_PopulateCommandList(CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& dsvHandle)
{
	World* world = Renderer::instance->GetWorld();
	if (!world) return;

	std::vector<Chunk*> chunks = world->GetWorldChunks();

	ChunkOpaqueRenderCommandList->BeginRecord(ChunkOpaquePSO.get()); 

	ChunkOpaqueRenderCommandList->Get()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	ChunkOpaqueRenderCommandList->Get()->RSSetViewports(1, &Renderer::instance->Viewport);
	ChunkOpaqueRenderCommandList->Get()->RSSetScissorRects(1, &Renderer::instance->ScissorRect);

	Camera* camera = Renderer::instance->GetCamera();
	camera->CalculateCameraMatrices();

	ChunkOpaqueRenderCommandList->Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i] == nullptr) continue;
		ChunkConstantBuffer CCB = {};

		XMStoreFloat4x4(&CCB.Projection, XMMatrixTranspose(camera->GetProjectionMatrix()));
		XMStoreFloat4x4(&CCB.View, XMMatrixTranspose(camera->GetViewMatrix()));
		XMMATRIX Model = XMMatrixTranslation(chunks[i]->ChunkPosition.X, 0, chunks[i]->ChunkPosition.Y);
		XMStoreFloat4x4(&CCB.Model, XMMatrixTranspose(Model));

		ChunkOpaqueRenderCommandList->Get()->SetGraphicsRootSignature(ChunkRootSignature.get()->GetRootSignature().Get());

		ID3D12DescriptorHeap* heaps[] = {
			ChunkBindingTable->GetDescriptorHeap(),
		};

		ChunkOpaqueRenderCommandList->Get()->SetDescriptorHeaps(_countof(heaps), heaps);

		ChunkOpaqueRenderCommandList->Get()->SetGraphicsRootDescriptorTable(0, ChunkBindingTable->GetGpuHandle(chunks[i]->ConstantBufferResource->CBVHandle));
		ChunkOpaqueRenderCommandList->Get()->SetGraphicsRootDescriptorTable(1, ChunkBindingTable->GetGpuHandle(ChunkVoxelAtlasTextureHandle));


		chunks[i]->ConstantBufferResource->UpdateConstantBufferData(&CCB, sizeof(CCB));

		auto vb = chunks[i]->ChunkVertexBuffer.GetBufferView();
		auto ib = chunks[i]->ChunkIndexBuffer.GetIndexBufferView();
		ChunkOpaqueRenderCommandList->Get()->IASetVertexBuffers(0, 1, &vb);
		ChunkOpaqueRenderCommandList->Get()->IASetIndexBuffer(&ib);
		ChunkOpaqueRenderCommandList->Get()->DrawIndexedInstanced(chunks[i]->ChunkIndexBuffer.GetIndicesCount(), 1, 0, 0, 0);
		
	}

	ChunkOpaqueRenderCommandList->EndRecord();



}
