#pragma once

#ifndef UNICODE
#define UNICODE
#endif 
#include <windows.h>

class Camera;
class Renderer;
class InputSystem;
class ResourceManager;
class ChunkRenderer;
class World;
class Config;

enum WindowMode : unsigned char
{
	Windowed = 0,
	BorderlessWindow = 1,
	Fullscreen = 2
};

class Engine
{
public:

	Engine(HWND hwnd,HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

	void EngineUpdate();

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetWindowMode(WindowMode NewWindowMode);

protected:

	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	float DeltaTime = 0.0f;

	HWND EngineWindowHWND;
	Config* EngineConfig = nullptr;
	Camera* EngineCamera = nullptr;
	Renderer* EngineRenderer = nullptr;
	InputSystem* EngineInputSystem = nullptr;
	ResourceManager* EngineResourceManager = nullptr;
	ChunkRenderer* EngineChunkRenderer = nullptr;
	World* EngineWorld = nullptr;


};
