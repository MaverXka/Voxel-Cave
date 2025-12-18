#pragma once

#include "Core/CoreTypes.h"
#include <string>
#include "../Resources/RenderResource.h"

enum DXGI_FORMAT;

class BindingTable;
class BindingTableAllocator;
struct BindingHandle;

class Texture : public RenderResource
{
public:

	inline int GetTexSizeX() { return TextureSize.X; }
	inline int GetTexSizeY() { return TextureSize.Y; }

	Texture(std::string FileName, ID3D12GraphicsCommandList* CommandList);

	ComPtr<ID3D12DescriptorHeap> GetSRV()
	{
		return M_TexSRVHeap;
	}

	void CreateShaderResourceView(BindingTable* InBindingTable, int Offset);
	BindingHandle CreateShaderResourceView(BindingTableAllocator* InBindingTableAllocator);

protected:

	friend class ResourceManager;



	DXGI_FORMAT TextureFormat;

	Vector2Int TextureSize;
	uint8 TextureChannelCount = 0;

	ComPtr<ID3D12DescriptorHeap> M_TexSRVHeap;
	ComPtr<ID3D12Resource> M_TextureUploadBuffer;

};