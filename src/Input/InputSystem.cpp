#include "InputSystem.h"

#include <format>
#include <Windows.h>
#include "Debug/Log/Logger.h"

InputSystem* InputSystem::Instance = nullptr;

InputSystem::InputSystem(HWND hwnd) : Window(hwnd)
{
	if (Instance == nullptr)
	{
		Instance = this;
	}

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hwnd;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		Logger::Get()->Log("Failed to register mouse.");
	}


}

InputSystem::~InputSystem()
{
}

void InputSystem::OnKeyUp(char Key)
{
	//Logger::Get()->LogFormat("Key up: {}", Key);
	PressedKeys.erase(Key);
}

void InputSystem::OnKeyDown(char Key)
{
	//Logger::Get()->LogFormat("Key down: {}", Key);
	PressedKeys.insert(Key);
}

bool InputSystem::IsKeyDown(char Key)
{
	return PressedKeys.contains(Key);
}

void InputSystem::SetCursorLock(bool Locked)
{
	requireCursorLock = Locked;
}

void InputSystem::ResetInput()
{
	PressedKeys.clear();
	MouseDelta = { 0, 0 };
}

void InputSystem::OnEngineWindowFocusChanged(bool Active)
{
	if (requireCursorLock && Active)
	{
		ShowCursor(false);
		isCursorLocked = true;
		WindowCenter = { 0, 0 };

		RECT rc;
		if (GetClientRect(Window, &rc))
		{
			POINT lt = { rc.left, rc.top };
			POINT rb = { rc.right, rc.bottom };
			ClientToScreen(Window, &lt);
			ClientToScreen(Window, &rb);
			RECT clip = { lt.x, lt.y, rb.x, rb.y };
			ClipCursor(&clip);
		}
	}
	else
	{
		ShowCursor(true);
		ClipCursor(nullptr);
		isCursorLocked = false;
	}
	ResetInput();

}

void InputSystem::InputUpdate()
{
	
}
