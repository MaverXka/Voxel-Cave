#pragma once

#include <DirectXMath.h>
#include "Core/CoreTypes.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void CalculateCameraMatrices();

	XMMATRIX GetViewMatrix() { return ViewMatrix; };
	XMMATRIX GetProjectionMatrix() { return ProjectionMatrix; };

	void SetCameraPosition(Vector3f NewPosition);

	void AddCameraOffset(Vector3f Offset) { SetCameraPosition(CameraPosition + Offset); };

	void AddCameraRotationDelta(Vector2Int Input);

	Vector3f GetCameraDir();
	Vector3f GetCameraPosition() { return CameraPosition; }

private:

	friend class Renderer;

	
	float HorizontalAngle = 0;
	float VerticalAngle = 0;

	void SetCameraDisplayResolution(int SizeX, int SizeY);

	int WindowSizeX = 0;
	int WindowSizeY = 0;

	Vector3f CameraPosition;
	Vector3f CameraForward;

	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

};