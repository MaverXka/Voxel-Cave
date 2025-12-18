#include "Engine.h"

#include <format>
#include <string>
#include <chrono>
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Input/InputSystem.h"
#include "World/World.h"
#include "Debug/Log/Logger.h"
#include "Resources/ResourceManager.h"
#include "Rendering/ChunkRenderer.h"
#include "Physics/AABB.h"
#include "Config.h"

#ifdef _DEBUG
#include "Debug/ImGUI/imgui_impl_win32.h"
#include "Debug/ImGUI/imgui_impl_dx12.h"
#include "Debug/ImGUI/imgui.h"

#endif

Engine::Engine(HWND hwnd,HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) : EngineWindowHWND(hwnd)
{
    EngineConfig = new Config();

    EngineInputSystem = new InputSystem(hwnd);
    EngineRenderer = new Renderer(hwnd);

    EngineWorld = new World();
    EngineCamera = new Camera();
    EngineRenderer->SetCamera(EngineCamera);
    EngineRenderer->SetRenderWorld(EngineWorld);
    EngineInputSystem->SetCursorLock(false);
    SetWindowTextW(hwnd, L"Voxel Cave | D3D12");
    EngineRenderer->SetRenderPaused(true);
    EngineResourceManager = new ResourceManager();
    EngineChunkRenderer = new ChunkRenderer();
    EngineRenderer->MainChunkRenderer = EngineChunkRenderer;
}

using aclock = std::chrono::high_resolution_clock;
using aseconds = std::chrono::duration<float>;

Vector3f PrevCamPos;
Vector3f MoveVel(0,0,0);

void Engine::EngineUpdate()
{
    static auto prev = aclock::now();
    auto now = aclock::now();
    aseconds dt = now - prev;
    prev = now;
    DeltaTime = dt.count();


    EngineInputSystem->InputUpdate();

    EngineWorld->Tick(DeltaTime);

    EngineCamera->AddCameraRotationDelta(EngineInputSystem->GetMouseInputDelta());

    PrevCamPos = EngineCamera->GetCameraPosition();

    Vector3f CamDir = EngineCamera->GetCameraDir();
    Vector3f CamUp = { 0,1,0 };
    Vector3f Right = Vector3f::Cross(CamDir, CamUp).Normalized();
    float camspeed = 3.0f;

    AABB chunkAABB;
    chunkAABB.Min = Vector3f(0, 0, 0);
    chunkAABB.Max = Vector3f(16, 3, 16);

    if (EngineInputSystem->IsKeyDown('W')) MoveVel += CamDir;
    if (EngineInputSystem->IsKeyDown('S')) MoveVel -= CamDir;
    if (EngineInputSystem->IsKeyDown('D')) MoveVel += Right;
    if (EngineInputSystem->IsKeyDown('A')) MoveVel -= Right;
    if (EngineInputSystem->IsKeyDown('Q')) MoveVel -= CamUp;
    if (EngineInputSystem->IsKeyDown('E')) MoveVel += CamUp;

    EngineCamera->AddCameraOffset(MoveVel.Normalized() * DeltaTime * camspeed);

    AABB cameraAABB;
    cameraAABB.Min = (EngineCamera->GetCameraPosition() - Vector3f(0.2F));
    cameraAABB.Max = (EngineCamera->GetCameraPosition() + Vector3f(0.2F));

    if (cameraAABB.IntersectOtherAABBV2(chunkAABB))
    {
        EngineCamera->SetCameraPosition(PrevCamPos);
    }

    MoveVel = 0;

    EngineInputSystem->MouseDelta = { 0,0 };
}

LRESULT CALLBACK Engine::StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }

    Engine* engine = reinterpret_cast<Engine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (engine)
    {
        return engine->WindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Engine::SetWindowMode(WindowMode NewWindowMode)
{
    switch (NewWindowMode)
    {
    case Windowed:
        
        break;
    case BorderlessWindow:
        SetWindowLongPtr(EngineWindowHWND, GWL_STYLE, WS_POPUP);
        SetWindowLongPtr(EngineWindowHWND, GWL_EXSTYLE, WS_EX_APPWINDOW);
        RECT rect;
        GetWindowRect(GetDesktopWindow(), &rect);
        SetWindowPos(
            EngineWindowHWND,
            HWND_TOP,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_SHOWWINDOW
        );
        break;
    case Fullscreen:
        break;
    default:
        break;
    }
}

#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT Engine::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

#ifdef _DEBUG
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
    {
        return true;
    }
#endif

    switch (uMsg)
    {
    case WM_DESTROY:
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
    {
        Logger::Get()->LogString(std::format("Window resized: {} {}", LOWORD(lParam), HIWORD(lParam)));
        EngineRenderer->Resize(LOWORD(lParam), HIWORD(lParam), false);
        return 0;
    }
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) != WA_INACTIVE)
        {
            EngineInputSystem->OnEngineWindowFocusChanged(true);
            Logger::Get()->LogString("Active");
        }
        else
        {
            EngineInputSystem->OnEngineWindowFocusChanged(false);
            Logger::Get()->LogString("Inactive");
        }
        return 0;
    }
    case WM_KEYDOWN:
    {
        EngineInputSystem->OnKeyDown(wParam);
        break;
    }
    case WM_KEYUP:
    {
        EngineInputSystem->OnKeyUp(wParam);
        break;
    }
    case WM_INPUT:
    {
        UINT size = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
        if (size == 0) break;
        std::vector<BYTE> buf(size);
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER)))
        {
            RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buf.data());
            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                LONG dx = raw->data.mouse.lLastX;
                LONG dy = raw->data.mouse.lLastY;
                EngineInputSystem->MouseDelta = { dx, dy };
                //Logger::Get()->LogFormat("Mouse input: {} {}",dx,dy);
            }
        }
        break;
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
