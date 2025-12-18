#pragma once
#include <vector>
#include "Core/CoreTypes.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/BindingTableAllocator.h"
#include "Rendering/VertexBuffer.h"

constexpr inline int CHUNK_WIDTH = 16;
constexpr inline int CHUNK_HEIGHT = 256;
constexpr int CHUNK_TOTAL_BLOCKS = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

class ConstantBuffer;

/// <summary>
/// Используеться для определения типа блока. В пределах от 0 до 255
/// </summary>
typedef unsigned char BlockID;

class Chunk
{
public:

	Chunk(Vector2Int ChunkPos);

	void Generate();

	/// <summary>
	/// Помечает что кусок требуется пересобрать.
	/// </summary>
	void MarkAsDirty()
	{
		IsChunkDirty = true;
	}

	int GetBlockIndex(int X, int Y, int Z)
	{
		return X + Y * CHUNK_WIDTH + Z * CHUNK_WIDTH * CHUNK_HEIGHT;
	}

	Vector3Int GetBlockPosFromIndex(int Idx)
	{
		const int W = CHUNK_WIDTH;
		const int H = CHUNK_HEIGHT;

		return Vector3Int(Idx % W, (Idx / W) % H, Idx / (W * H));
	}

	VertexBuffer ChunkVertexBuffer;
	IndexBuffer  ChunkIndexBuffer;
	Vector2Int ChunkPosition = { 0,0 };
	
	void MakeConstantBuffer(BindingTableAllocator* InAllocator);

	ConstantBuffer* ConstantBufferResource;

	BlockID ChunkBlocks[CHUNK_TOTAL_BLOCKS];

protected:

	friend class World;

	void GenerateChunkMesh();

	void GenerateBlock(int x, int y, int z);

	bool IsBlocked(int x, int y, int z);
	BlockID GetBlockAtPos(Vector3Int BlockPosition);

	void GenLeftSide(Vector3Int BlockPos);
	void GenRightSide(Vector3Int BlockPos);
	void GenFrontSide(Vector3Int BlockPos);
	void GenBackSide(Vector3Int BlockPos);
	void GenTopSide(Vector3Int BlockPos);
	void GenBottomSide(Vector3Int BlockPos);

	void GenerateLastIndices();

	std::vector<PositionUVColorVertex> Vertices;
	std::vector<int> Indices;

	
	bool IsChunkDirty = true;

};


