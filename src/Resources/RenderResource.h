#pragma once

#include "Resource.h"
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"


using namespace Microsoft::WRL;

class RenderResource : public Resource
{
public:
	RenderResource();
	~RenderResource();

protected:

	ComPtr<ID3D12Resource> M_Resource;

};