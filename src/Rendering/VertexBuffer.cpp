#include "VertexBuffer.h"
#include <map>
#include <format>
#include <d3dx12.h>
#include "../Rendering/Renderer.h"
#include "Debug/Log/Logger.h"


const std::map<VertexFormat, const char*> FormatToString =
{
	{ VertexFormat::PositionUV, "Position4 TexCoord2" },
	{ VertexFormat::PositionColor, "Position4 Color4" },
	{ VertexFormat::PositionUVColor, "Position4 TexCoord2 Color4" }
};

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(VertexFormat vertexFormat)
{
	bufferVertexFormat = vertexFormat;
	auto D3DDevice = Renderer::instance->M_D3D12Device;
	Logger::Get()->LogString(std::format("Creating a new vertex buffer with format: {}", FormatToString.at(vertexFormat)));

}

void VertexBuffer::UploadData(size_t Size, void* NewData)
{
	Data = NewData;
	BufferSize = Size;
}

void VertexBuffer::UploadVertices(int vertexCount, size_t StrideSize, void* NewData)
{
	VertexCount = vertexCount;
	Data = NewData;
	BufferSize = vertexCount *StrideSize;
}

void VertexBuffer::Build()
{
	if (Assembled)
	{
		Logger::Get()->Log("Vertex buffer is already assembled.");
		return;
	}
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vertexbufferdesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	auto D3DDevice = Renderer::instance->M_D3D12Device;

	if (FAILED(D3DDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vertexbufferdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&BufferResource))))
	{
		Logger::Get()->Log("D3D12 Buffer resource creation failed.");
	}

	CD3DX12_RANGE readRange(0, 0);
	if (FAILED(BufferResource->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin))))
	{
		Logger::Get()->Log("Vertex buffer resource map failed.");
	}

	memcpy(pDataBegin, Data, BufferSize);
	BufferResource->Unmap(0, nullptr);

	VertexBufferView.BufferLocation = BufferResource->GetGPUVirtualAddress();
	VertexBufferView.SizeInBytes = BufferSize;
	switch (bufferVertexFormat)
	{
	case PositionColor:
		VertexBufferView.StrideInBytes = sizeof(PositionColorVertex);
		break;
	case PositionUVColor:
		VertexBufferView.StrideInBytes = sizeof(PositionUVColorVertex);
		break;
	case PositionUV:
		VertexBufferView.StrideInBytes = sizeof(PositionColorVertex);
		break;
	default:
		break;
	}
	Logger::Get()->LogString(std::format("D3D12 vertex buffer stride: {}", VertexBufferView.StrideInBytes));

	Logger::Get()->LogString(std::format("D3D12 vertex buffer uploaded. Size: {}", BufferSize));
	Assembled = true;
}
