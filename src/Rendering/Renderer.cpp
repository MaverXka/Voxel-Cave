#include "Renderer.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <exception>
#include <DirectXMath.h>
#include <condition_variable>
#include <mutex>
#include "d3dx12.h"
#include "Debug/Log/Logger.h"
#include "PSO.h"
#include "RootSignature.h"
#include "Texture.h"
#include "Core/Thread.h"
#include "Shader.h"
#include "Camera.h"
#include "World/Chunk.h"
#include "Resources/ResourceManager.h"
#include "World/World.h"
#include "Rendering/VertexBuffer.h"
#include "ChunkRenderer.h"

#ifdef _DEBUG
#include "Debug/ImGUI/ImGUIManager.h"
#include "Debug/ImGUI/imgui_impl_dx12.h"
#include "Debug/ImGUI/imgui_impl_win32.h"
#endif

#include "GraphicsCommandList.h"


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

std::atomic<bool> RenderThreadPauseRequested = false;
std::atomic<bool> RenderThreadPaused = false;
std::mutex RenderThreadMutex;
std::condition_variable RenderThreadCV;

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(HWND hwnd) : Viewport(0.0f,0.0f,1280.0f,720.0f), ScissorRect(0.0f, 0.0f, 1280.0f, 720.0f)
{
	if (!instance)
	{
		instance = this;
	}

	Logger::Get()->Log("Renderer initializing.");

	int FactoryFlags = 0;
#ifdef DEBUG
	FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
    {
		DebugController->EnableDebugLayer();
    }
#endif


	if (SUCCEEDED(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&DXGIFactory))))
	{
		Logger::Get()->Log("DXGI Factory created successfully.");
	}
	else { Logger::Get()->Log("Failed to create DXGI Factory."); }

	if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&M_D3D12Device))))
	{
		Logger::Get()->Log("D3D12 Device created successfully.");
	}


	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.NodeMask = 0;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	if (SUCCEEDED(M_D3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&M_MainCommandQueue))))
	{
		Logger::Get()->Log("D3D12 command queue created.");
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.Flags = 0;
	ComPtr<IDXGISwapChain1> swapchain = nullptr;
	if (SUCCEEDED(DXGIFactory->CreateSwapChainForHwnd(M_MainCommandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapchain)))
	{
		Logger::Get()->Log("DXGI SwapChain created.");
	}
	swapchain.As(&DXGISwapChain);

	FrameIndex = DXGISwapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(M_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&M_RTVHeap))))
	{
		Logger::Get()->Log("Render target view heap creation error.");
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(M_D3D12Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&M_DSVHeap))))
	{
		Logger::Get()->Log("Depth stencil view heap creation error.");
	}

	RTV_DescriptorSize = M_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RTVHeap->GetCPUDescriptorHandleForHeapStart());
		for (int n = 0; n < FrameCount; n++)
		{
			if (FAILED(DXGISwapChain->GetBuffer(n, IID_PPV_ARGS(&M_RenderTargets[n]))))
			{
				Logger::Get()->Log("Cannot get a render target buffer.");
			}
			M_D3D12Device->CreateRenderTargetView(M_RenderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += RTV_DescriptorSize;
		}
		rtvCPUHandle = rtvHandle;
	}


	D3D12_HEAP_PROPERTIES deapthHeapPropeprties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WindowSizeX, WindowSizeY, 1, 1);
	depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	CD3DX12_CLEAR_VALUE depthValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0.0);

	M_D3D12Device->CreateCommittedResource(
		&deapthHeapPropeprties,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthValue,
		IID_PPV_ARGS(&M_DSVTexture)
	);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	M_D3D12Device->CreateDepthStencilView(M_DSVTexture.Get(), &dsvDesc, M_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	

	if (SUCCEEDED(M_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&M_MainCommandAllocator))))
	{
		Logger::Get()->Log("Main command allocator created.");
	}

	M_D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&M_Fence));
	fenceValue = 1;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), "Fence event failed.");
	}

	WaitForPreviousFrame();

	FrameBeginCommandList = new GraphicsCommandList(M_D3D12Device.Get());
	FrameEndCommandList = new GraphicsCommandList(M_D3D12Device.Get());

#ifdef _DEBUG
	MainImGUIManager = new ImGUIManager(hwnd);
#endif

	RenderThread = Thread::Create([&]() { Render(); }, "Render");


}


void Renderer::Render()
{
	Logger::Get()->Log("Render thread init.");
	while (true)
	{
		FrameBeginCommandList->BeginRecord();
		auto dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(M_DSVHeap->GetCPUDescriptorHandleForHeapStart());
		auto b1 = CD3DX12_RESOURCE_BARRIER::Transition(M_RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		FrameBeginCommandList->Get()->ResourceBarrier(1, &b1);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RTVHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RTV_DescriptorSize);
		const float clearColor[] = { 0.0f, 0.05f, 0.2f, 1.0f };
		FrameBeginCommandList->Get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		FrameBeginCommandList->Get()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0, 0, nullptr);
		FrameBeginCommandList->EndRecord();

		if (MainChunkRenderer)
		{
			MainChunkRenderer->RenderThread_PopulateCommandList(rtvHandle, dsvHandle);
		}


		FrameEndCommandList->BeginRecord();
		auto b2 = CD3DX12_RESOURCE_BARRIER::Transition(M_RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		FrameEndCommandList->Get()->ResourceBarrier(1, &b2);
		FrameEndCommandList->EndRecord();

#ifdef _DEBUG
		bool dshow = true;
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		ImGui::ShowDemoWindow(&dshow);
#endif

		{
			ImGui::Render();
		}

		MainImGUIManager->RenderThread_PopulateCommandList(rtvHandle);

		if (MainChunkRenderer)
		{
			ID3D12CommandList* ppCommandLists[] = { FrameBeginCommandList->Get(), MainChunkRenderer->ChunkOpaqueRenderCommandList->Get(),MainImGUIManager->ImGuiCommandList->Get() ,FrameEndCommandList->Get() };
			M_MainCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
		else
		{
			ID3D12CommandList* ppCommandLists[] = { FrameBeginCommandList->Get(), MainImGUIManager->ImGuiCommandList->Get(),FrameEndCommandList->Get() };
			M_MainCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}


		DXGISwapChain->Present(1, 0);

		WaitForPreviousFrame();

		if (RenderThreadPauseRequested.load(std::memory_order_acquire))
		{
			std::unique_lock<std::mutex> lock(RenderThreadMutex);
			RenderThreadPaused.store(true, std::memory_order_release);
			RenderThreadCV.notify_one();
			RenderThreadCV.wait(lock, [] { return !RenderThreadPauseRequested.load(std::memory_order_acquire); });
			RenderThreadPaused.store(false, std::memory_order_release);
		}

	}
}

void Renderer::WaitForPreviousFrame()
{
	const UINT64 fence = fenceValue;
	M_MainCommandQueue->Signal(M_Fence.Get(), fence);
	fenceValue++;

	if (M_Fence->GetCompletedValue() < fence)
	{
		M_Fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	FrameIndex = DXGISwapChain->GetCurrentBackBufferIndex();
}

void Renderer::WaitForRenderThread()
{

}

void Renderer::Resize(int SizeX, int SizeY, bool Fullscreen)
{
	WindowSizeX = SizeX;
	WindowSizeY = SizeY;
	if (WindowSizeX <= 0 || WindowSizeY <= 0) return;
	Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, WindowSizeX, WindowSizeY);
	ScissorRect = CD3DX12_RECT(0.0f, 0.0f, WindowSizeX, WindowSizeY);
	//WaitForRenderThread();
	//RenderThreadPauseRequested.store(true, std::memory_order_release);
	//{
	//	std::unique_lock<std::mutex> lock(RenderThreadMutex);
	//	RenderThreadCV.wait(lock, [] { return RenderThreadPaused.load(std::memory_order_acquire); });
	//}
	SetRenderPaused(true);
	//WaitForPreviousFrame();

	for (int i = 0; i < FrameCount; i++)
	{
		M_RenderTargets[i].Reset();
	}
	DXGI_SWAP_CHAIN_DESC desc = {};
	DXGISwapChain->GetDesc(&desc);
	DXGISwapChain->ResizeBuffers(FrameCount, SizeX, SizeY, DXGI_FORMAT_UNKNOWN, desc.Flags);
	
	M_DSVTexture.Reset();

	D3D12_HEAP_PROPERTIES deapthHeapPropeprties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WindowSizeX, WindowSizeY, 1, 1);
	depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	CD3DX12_CLEAR_VALUE depthValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0.0);

	M_D3D12Device->CreateCommittedResource(
		&deapthHeapPropeprties,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthValue,
		IID_PPV_ARGS(&M_DSVTexture)
	);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	M_D3D12Device->CreateDepthStencilView(M_DSVTexture.Get(), &dsvDesc, M_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	FrameIndex = DXGISwapChain->GetCurrentBackBufferIndex();

	CreateSwapChainBuffers();

	CameraObject->SetCameraDisplayResolution(WindowSizeX, WindowSizeY);

	BOOL fullscreenState;
	DXGISwapChain->GetFullscreenState(&fullscreenState, nullptr);

	SetRenderPaused(false);
	//RenderThreadPauseRequested.store(false, std::memory_order_release);
	//RenderThreadCV.notify_one();

}

void Renderer::SetRenderWorld(World* NewWorld)
{
	RenderWorld = NewWorld;
}

void Renderer::SetCamera(Camera* NewCamera)
{
	CameraObject = NewCamera; CameraObject->SetCameraDisplayResolution(WindowSizeX, WindowSizeY);
}

void Renderer::CreateSwapChainBuffers()
{
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(M_RTVHeap->GetCPUDescriptorHandleForHeapStart());
		for (int n = 0; n < FrameCount; n++)
		{
			if (FAILED(DXGISwapChain->GetBuffer(n, IID_PPV_ARGS(&M_RenderTargets[n]))))
			{
				Logger::Get()->Log("Cannot get a render target buffer.");
			}
			M_D3D12Device->CreateRenderTargetView(M_RenderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += RTV_DescriptorSize;
		}
		rtvCPUHandle = rtvHandle;
	}
}

void Renderer::SetRenderPaused(bool isPaused)
{
	//Logger::Get()->Log("Render pause change.");
	if (isPaused)
	{
		RenderThreadPauseRequested.store(true, std::memory_order_release);
		{
			std::unique_lock<std::mutex> lock(RenderThreadMutex);
			RenderThreadCV.wait(lock, [] { return RenderThreadPaused.load(std::memory_order_acquire); });
		}
		WaitForPreviousFrame();
	}
	else
	{
		RenderThreadPauseRequested.store(false, std::memory_order_release);
		RenderThreadCV.notify_one();
	}


}
