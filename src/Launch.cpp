#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include <windows.h>
#include "Engine/Engine.h"
#include "Debug/Log/Logger.h"
#include "Core/Thread.h"



static Engine* MainEngine = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Thread::AssignMainThreadName();

#ifdef DEBUG
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Voxel Cave Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = Engine::StaticWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Voxel Cave",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,
        NULL,
        hInstance,
        MainEngine
    );

    if (hwnd == NULL)
    {
        Logger::Get()->Log("Window creation failed.");
        return 0;
    }
    else
    {
        Logger::Get()->Log("Window created successfully.");
    }


    MainEngine = new Engine(hwnd,hInstance, hPrevInstance, pCmdLine, nCmdShow);

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(MainEngine));
    ShowWindow(hwnd, nCmdShow);

    MainEngine->SetWindowMode(WindowMode::BorderlessWindow);

    bool running = true;
    MSG msg = { };
    while (running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        MainEngine->EngineUpdate();

    }

    return 0;
}