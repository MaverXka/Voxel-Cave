#pragma once

#include <Windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "Core/Thread.h"
#include "GraphicsCommandList.h"
#include <atomic>

class PSO;
class RootSignature;
class World;
class Camera;
class ChunkRenderer;

class Engine;

#ifdef _DEBUG
class ImGUIManager;
class GizmoDrawingManager;
#endif

using namespace Microsoft::WRL;
using namespace DirectX;



struct ChunkConstantBuffer
{
    XMFLOAT4X4 Projection;
    XMFLOAT4X4 View;
    XMFLOAT4X4 Model;
    float padding[16];
};
static_assert((sizeof(ChunkConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

class Renderer
{
public:

	explicit Renderer(HWND hwnd);

	void Render();
	
	ChunkRenderer* MainChunkRenderer = nullptr;

	ComPtr<ID3D12Fence> M_Fence;
	int fenceValue = 0;
	HANDLE m_fenceEvent;
	void WaitForPreviousFrame();

	void WaitForRenderThread();

	Thread RenderThread;

	GraphicsCommandList* FrameBeginCommandList;
	GraphicsCommandList* FrameEndCommandList;

#ifdef _DEBUG
	ImGUIManager* MainImGUIManager;
	GizmoDrawingManager* MainGizmoDrawManager;
#endif

	ComPtr<ID3D12Debug> DebugController;
	ComPtr<IDXGIFactory7> DXGIFactory;
	ComPtr<IDXGISwapChain3> DXGISwapChain;
	ComPtr<ID3D12Device14> M_D3D12Device;
	ComPtr<ID3D12CommandQueue> M_MainCommandQueue;
	ComPtr<ID3D12CommandAllocator> M_MainCommandAllocator;
	ComPtr<ID3D12DescriptorHeap> M_RTVHeap;
	ComPtr<ID3D12DescriptorHeap> M_DSVHeap;
	ComPtr<ID3D12Resource> M_DSVTexture;
	Engine* EnginePtr;

	std::vector<ID3D12CommandList*> Render_CommandLists;

	int FrameIndex = 0;
	static const int FrameCount = 2;
	int RTV_DescriptorSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;
	ComPtr<ID3D12Resource> M_RenderTargets[FrameCount];
	CD3DX12_VIEWPORT Viewport;
	CD3DX12_RECT ScissorRect;

	static Renderer* instance;

	void Resize(int SizeX, int SizeY, bool Fullscreen);

	void SetRenderWorld(World* NewWorld);
	World* GetWorld()
	{
		return RenderWorld;
	}

	Camera* GetCamera()
	{
		return CameraObject;
	}

	void SetCamera(Camera* NewCamera);

	void CreateSwapChainBuffers();

	int WindowSizeX = 1280;
	int WindowSizeY = 720;

	void SetRenderPaused(bool isPaused);

protected:


	World* RenderWorld = nullptr;
	Camera* CameraObject;


};


