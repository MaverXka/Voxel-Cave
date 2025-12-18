#include "ResourceManager.h"
#include "Debug/Log/Logger.h"
#include <filesystem>
#include "Resource.h"
#include <string_view>
#include "Rendering/Renderer.h"
#include "Rendering/Texture.h"
#include "Resource.h"
#include "Rendering/Shader.h"
#include <any>

namespace fs = std::filesystem;

struct PathResource
{
	PathResource() : path(), InResource()
	{

	}

	PathResource(fs::path inPath, std::shared_ptr<Resource> Resource) : path(inPath), InResource(Resource)
	{

	}

	fs::path path;
	std::shared_ptr<Resource> InResource;
};

static constexpr std::string_view AllowedResourceExtensions[] =
{
	".hlsl",
	".png",
	".jpg"
};

static constexpr std::string_view ResourcePrefixes[] =
{
	"TEX_",
	"SHADER_"
	"SFX_"
};

std::vector<PathResource> PendingResources;

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
	if (!instance)
	{
		instance = this;
	}
	auto Device = Renderer::instance->M_D3D12Device;

	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&ResourceCommandAllocator)), "Failed to create resource command allocator.")
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ResourceCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&ResourceCommandList)), "Failed to create resource command list.")
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&ResourceUploadFence)), "Failed to create resource upload fence.")

	fs::path assetsPath = fs::current_path() / "Assets/";
	Logger::Get()->LogFormat("Resource manager working dir: {}", assetsPath.generic_string());

	for (auto const& entry : fs::recursive_directory_iterator(assetsPath))
	{
		if (!entry.is_regular_file())
			continue;

		auto ext = entry.path().extension().generic_string();
		for (auto const& allowed : AllowedResourceExtensions)
		{
			if (ext == allowed)
			{
				Logger::Get()->LogFormat("Found resource: {}", entry.path().filename().generic_string());
				if (ext == ".png")
				{
					std::shared_ptr<Texture> resourceTexture = make_shared<Texture>(entry.path().string(), ResourceCommandList.Get());
					PathResource pr;
					pr.path = entry;
					pr.InResource = resourceTexture;
					PendingResources.push_back(pr);
				}
				else if (ext == ".hlsl")
				{
					//std::shared_ptr<Shader> resourceTexture = make_shared<Shader>();
				}
				break;
			}
		}
	}
	ThrowIfFailed(ResourceCommandList->Close(), "Failed to close resource upload command list.")
	UploadResources();
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::UploadResources()
{
	auto CommandQueue = Renderer::instance->M_MainCommandQueue;

	ID3D12CommandList* Lists[] = { ResourceCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(Lists), Lists);
	fenceVal++;
	CommandQueue->Signal(ResourceUploadFence.Get(), fenceVal);
	HANDLE ruh = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	ResourceUploadFence->SetEventOnCompletion(fenceVal, ruh);
	WaitForSingleObject(ruh, INFINITE);
	CloseHandle(ruh);

	for (int i = 0; i < PendingResources.size(); i++)
	{
		std::string prefix;

		Texture* texture = reinterpret_cast<Texture*>(PendingResources[i].InResource.get());
		if (texture)
		{
			prefix = ResourcePrefixes[0];
		}

		std::string resourceName = prefix + PendingResources[i].path.filename().replace_extension("").generic_string();

		ResourceMap[resourceName] = PendingResources[i].InResource;
		Logger::Get()->LogFormat("Resource created: {} [{}]", resourceName, typeid(*PendingResources[i].InResource.get()).name());
	
	}
	PendingResources.clear();

	Logger::Get()->Log("Resources upload complete.");
}

std::shared_ptr<Resource> ResourceManager::GetResource(std::string resource)
{
	ResourceManager* manager = ResourceManager::instance;
	return manager->ResourceMap[resource];
}
