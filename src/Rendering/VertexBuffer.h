#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include "Core/CoreTypes.h"
#include "Buffer.h"


struct PositionColorVertex
{
	float Position[3];
	float Color[4];
};

struct PositionUVColorVertex
{
	PositionUVColorVertex(Vector3f pos, Vector2f uv, Vector4f col)
	{
		Position[0] = pos.X;
		Position[1] = pos.Y;
		Position[2] = pos.Z;
		TexCoord[0] = uv.X;
		TexCoord[1] = uv.Y;
		Color[0] = col.X;
		Color[1] = col.Y;
		Color[2] = col.Z;
		Color[3] = col.W;
	}
	float Position[3];
	float TexCoord[2];
	float Color[4];
};

enum VertexFormat : int
{
	PositionUV,
	PositionUVColor,
	PositionColor,
};

const D3D12_INPUT_ELEMENT_DESC PositionUVColorElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
};

using namespace Microsoft::WRL;

class VertexBuffer : public Buffer
{
public:

	VertexBuffer();
	VertexBuffer(VertexFormat vertexFormat);

	virtual void UploadData(size_t Size, void* NewData) override;
	void UploadVertices(int vertexCount, size_t StrideSize, void* Data);

	D3D12_VERTEX_BUFFER_VIEW GetBufferView()
	{
		return VertexBufferView;
	}

	int GetVertexCount() { return VertexCount; }

	virtual void Build() override;

protected:

	int VertexCount = 0;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

	VertexFormat bufferVertexFormat;

};
