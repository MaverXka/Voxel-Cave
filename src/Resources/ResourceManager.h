#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <unordered_map>

using namespace Microsoft::WRL;

class Resource;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void UploadResources();

	static std::shared_ptr<Resource> GetResource(std::string resource);

protected:

	std::unordered_map<std::string, std::shared_ptr<Resource>> ResourceMap;

	ComPtr<ID3D12CommandAllocator> ResourceCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> ResourceCommandList;
	ComPtr<ID3D12Fence> ResourceUploadFence;
	UINT64 fenceVal = 0;

	static ResourceManager* instance;

};