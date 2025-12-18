#include "Camera.h"
#include <DirectXMath.h>
#include <format>
#include "Core/Utils.h"
#include "Debug/Log/Logger.h"

using namespace DirectX;


Camera::Camera()
{
	CameraPosition = Vector3f(0, 5, 0);
	CameraForward = Vector3f(0, 1, 0);
}

Camera::~Camera()
{
}
static float p = 0.0f;

void Camera::CalculateCameraMatrices()
{
	ProjectionMatrix = XMMatrixPerspectiveFovRH(70.0f*3.14f/180.0f, (float)WindowSizeX / (float)WindowSizeY, 0.01f, 100.0f);
	ViewMatrix = XMMatrixLookAtRH(Utils::FromD3DVector(CameraPosition), Utils::FromD3DVector(CameraPosition + GetCameraDir()), XMVectorSet(0, 1, 0, 0));
	p += 0.01;
}

void Camera::SetCameraPosition(Vector3f NewPosition)
{


	CameraPosition = NewPosition;
	//Logger::Get()->LogFormat("Camera pos X: {} Y: {} Z: {}", CameraPosition.X, CameraPosition.Y, CameraPosition.Z);

}

void Camera::AddCameraRotationDelta(Vector2Int Input)
{
	HorizontalAngle += Input.X * 0.0025f;
	VerticalAngle += Input.Y * -0.0025f;
}

Vector3f Camera::GetCameraDir()
{
	Vector3f dir;
	dir.X = cosf(VerticalAngle) * cosf(HorizontalAngle);
	dir.Y = sinf(VerticalAngle);
	dir.Z = cosf(VerticalAngle) * sinf(HorizontalAngle);
	dir = dir.Normalized();
	return dir;
}

void Camera::SetCameraDisplayResolution(int SizeX, int SizeY)
{
	WindowSizeX = SizeX;
	WindowSizeY = SizeY;
}
