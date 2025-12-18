#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include "Utils/stb_image.h"
#include <wrl.h>
#include <d3dx12.h>
#include <d3d12.h>
#include <fstream>
#include "Renderer.h"
#include "Debug/Log/Logger.h"
#include "BindingTableAllocator.h"
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

Texture::Texture(std::string FileName,ID3D12GraphicsCommandList* CommandList)
{

	ComPtr<ID3D12Device14> D3DDevice = Renderer::instance->M_D3D12Device;
	
	int ImageSizeX = 0;
	int ImageSizeY = 0;
	int ChannelCount = 0;

	unsigned char* Data = stbi_load(FileName.c_str(), &ImageSizeX, &ImageSizeY, &ChannelCount, 4);
	Logger::Get()->LogFormat("Creating texture with dimensions: {} x {} and channel count: {}.", ImageSizeX, ImageSizeY, ChannelCount);

	TextureSize = { ImageSizeX, ImageSizeY };
	TextureChannelCount = ChannelCount;

	D3D12_DESCRIPTOR_HEAP_DESC tsrvh = {};
	tsrvh.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	tsrvh.NumDescriptors = 1;
	tsrvh.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ThrowIfFailed(D3DDevice->CreateDescriptorHeap(&tsrvh, IID_PPV_ARGS(&M_TexSRVHeap)), "Failed to create texture srv descriptor heap.")

	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, ImageSizeX, ImageSizeY, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_NONE);

	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto uploadheap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	if (FAILED(D3DDevice->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&M_Resource))))
	{
		Logger::Get()->Log("Failed to create texture resource.");
		return;
	}

	UINT64 size =GetRequiredIntermediateSize(M_Resource.Get(), 0, 1);
	CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	if (FAILED(D3DDevice->CreateCommittedResource(&uploadheap, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&M_TextureUploadBuffer))))
	{
		Logger::Get()->Log("Failed to create upload texture buffer resource.");
		return;
	}

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &Data[0];
	textureData.RowPitch = ImageSizeX * ChannelCount;
	textureData.SlicePitch = textureData.RowPitch * ImageSizeY;
	TextureFormat = textureDesc.Format;

	UpdateSubresources(CommandList, M_Resource.Get(), M_TextureUploadBuffer.Get(), 0, 0, 1, &textureData);
	auto lbarrier = CD3DX12_RESOURCE_BARRIER::Transition(M_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	CommandList->ResourceBarrier(1, &lbarrier);




	//stbi_image_free(Data);
}

void Texture::CreateShaderResourceView(BindingTable* InBindingTable, int Offset)
{
	auto dheap = InBindingTable->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	dheap.ptr += static_cast<uint64_t>(Offset) * InBindingTable->GetIncSize();

	ComPtr<ID3D12Device14> D3DDevice = Renderer::instance->M_D3D12Device;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	D3DDevice->CreateShaderResourceView(M_Resource.Get(), &srvDesc, dheap);
}

BindingHandle Texture::CreateShaderResourceView(BindingTableAllocator* InBindingTableAllocator)
{
	BindingHandle handle = InBindingTableAllocator->Allocate();

	ComPtr<ID3D12Device14> D3DDevice = Renderer::instance->M_D3D12Device;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	D3DDevice->CreateShaderResourceView(M_Resource.Get(), &srvDesc, InBindingTableAllocator->GetCpuHandle(handle));
	return handle;
}
