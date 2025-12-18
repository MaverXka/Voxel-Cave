#pragma once

#include <set>
#include <Windows.h>
#include "Core/CoreTypes.h"


class InputSystem
{
public:

	InputSystem(HWND hwnd);
	~InputSystem();

	static InputSystem* Get()
	{
		return Instance;
	}

	void OnKeyUp(char Key);
	void OnKeyDown(char Key);

	bool IsKeyDown(char Key);

	void SetCursorLock(bool Locked);

	Vector2Int GetMouseInputDelta()
	{
		return MouseDelta;
	}

	void ResetInput();

private:

	friend class Engine;

	HWND Window;

	void OnEngineWindowFocusChanged(bool Active);

	void InputUpdate();

	Vector2Int WindowCenter;
	Vector2Int MouseDelta = { 0, 0 };

	std::set<char> PressedKeys = std::set<char>();

	bool requireCursorLock = false;
	bool isCursorLocked = false;

	static InputSystem* Instance;

};


