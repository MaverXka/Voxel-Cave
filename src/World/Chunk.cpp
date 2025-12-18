#include "Chunk.h"
#include "Rendering/Renderer.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/VertexBuffer.h"



constexpr float TILE = 16.0f / 256.0f; // 0.0625f

// 2-я плитка по X, 1-я строка по Y
constexpr float U0 = 1.0f * TILE;
constexpr float V0 = 0.0f * TILE;
constexpr float U1 = U0 + TILE;
constexpr float V1 = V0 + TILE;

Chunk::Chunk(Vector2Int ChunkPos) : ChunkPosition(ChunkPos*CHUNK_WIDTH)
{
	ChunkVertexBuffer = VertexBuffer(VertexFormat::PositionUVColor);

}

void Chunk::Generate()
{

	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_WIDTH; z++)
		{
			for (int y = 0; y < CHUNK_HEIGHT;y++)
			{
				if (y < 3)
				{
					ChunkBlocks[GetBlockIndex(x, y, z)] = 1;
				}
				else
				{
					ChunkBlocks[GetBlockIndex(x, y, z)] = 0;
				}
			}
		}
	}
    ChunkBlocks[GetBlockIndex(2, 4, 0)] = 1;
    ChunkBlocks[GetBlockIndex(2, 5, 0)] = 1;

}

void Chunk::MakeConstantBuffer(BindingTableAllocator* InAllocator)
{
    ConstantBufferResource = new ConstantBuffer(&InAllocator->Table());
    ChunkConstantBuffer CCB = {};
    ConstantBufferResource->UploadData(sizeof(CCB), &CCB);
    ConstantBufferResource->Build();

}

void Chunk::GenerateChunkMesh()
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                if (ChunkBlocks[GetBlockIndex(x, y, z)] == 1)
                {
                    GenerateBlock(x, y, z);
                }
            }
        }
    }

    ChunkVertexBuffer.UploadVertices(Vertices.size(), sizeof(PositionUVColorVertex), Vertices.data());
    ChunkVertexBuffer.Build();

    ChunkIndexBuffer.UploadIndices(Indices.size(),sizeof(int), Indices.data());
    ChunkIndexBuffer.Build();
}

void Chunk::GenerateLastIndices()
{
    Indices.push_back(Vertices.size()-4);
    Indices.push_back(Vertices.size()-3);
    Indices.push_back(Vertices.size()-2);

    Indices.push_back(Vertices.size()-3);
    Indices.push_back(Vertices.size()-1);
    Indices.push_back(Vertices.size()-2);
}

void Chunk::GenerateBlock(int x, int y, int z)
{
   // if (ChunkBlocks[GetBlockIndex(x, y, z)] == 0)
   // {
    //    return;
    //}

    if (GetBlockAtPos(Vector3Int(x+1,y,z)) == 0)
    {
        GenFrontSide(Vector3Int(x, y, z));
    }
    if (GetBlockAtPos(Vector3Int(x-1, y, z)) == 0)
    {
        GenBackSide(Vector3Int(x, y, z));
    }

    if (GetBlockAtPos(Vector3Int(x, y, z+1)) == 0)
    {
        GenRightSide(Vector3Int(x, y, z));
    }
    //GenRightSide(Vector3Int(x, y, z));

    if (GetBlockAtPos(Vector3Int(x, y, z-1)) == 0)
    {
        GenLeftSide(Vector3Int(x, y, z));
    }

    if (GetBlockAtPos(Vector3Int(x, y+1, z)) == 0)
    {
        GenTopSide(Vector3Int(x, y, z));
    }
    if (GetBlockAtPos(Vector3Int(x, y-1, z)) == 0)
    {
        GenBottomSide(Vector3Int(x, y, z));
    }


}

bool Chunk::IsBlocked(int x, int y, int z)
{
    if (x+1 > CHUNK_WIDTH || z+1 > CHUNK_WIDTH || x < 0 || z < 0 || z+1 > CHUNK_HEIGHT || z < 0)
    {
        return false;
    }

    if (ChunkBlocks[GetBlockIndex(x, y, z)] != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BlockID Chunk::GetBlockAtPos(Vector3Int BlockPosition)
{
    if (
        BlockPosition.X >= 0 && BlockPosition.X < CHUNK_WIDTH &&
        BlockPosition.Y >= 0 && BlockPosition.Y < CHUNK_HEIGHT &&
        BlockPosition.Z >= 0 && BlockPosition.Z < CHUNK_WIDTH
        )
    {
        return ChunkBlocks[GetBlockIndex(BlockPosition.X, BlockPosition.Y, BlockPosition.Z)];
    }
    else
    {
        return 0;
    }

}

void Chunk::GenLeftSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 0) + BlockPos, Vector2f(U0, V0), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 0) + BlockPos, Vector2f(U1, V0), Vector4f(0, 1, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 0) + BlockPos, Vector2f(U0, V1), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 0) + BlockPos, Vector2f(U1, V1), Vector4f(1, 1, 1, 0)));
    GenerateLastIndices();
}

void Chunk::GenRightSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 1) + BlockPos, Vector2f(U0, V0), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 1) + BlockPos, Vector2f(U1, V0), Vector4f(0, 1, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 1) + BlockPos, Vector2f(U0, V1), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 1) + BlockPos, Vector2f(U1, V1), Vector4f(1, 1, 1, 0)));
    GenerateLastIndices();
}

void Chunk::GenFrontSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 1) + BlockPos, Vector2f(U0, V0), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 1) + BlockPos, Vector2f(U1, V0), Vector4f(1, 1, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 0) + BlockPos, Vector2f(U0, V1), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 0) + BlockPos, Vector2f(U1, V1), Vector4f(0, 1, 0, 0)));
    GenerateLastIndices();
}

void Chunk::GenBackSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 0) + BlockPos, Vector2f(U0, V0), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 1) + BlockPos, Vector2f(U1, V0), Vector4f(1, 1, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 0) + BlockPos, Vector2f(U0, V1), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 1) + BlockPos, Vector2f(U1, V1), Vector4f(0, 1, 0, 0)));
    GenerateLastIndices();
}

void Chunk::GenTopSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 0) + BlockPos, Vector2f(U0, V0), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 1, 1) + BlockPos, Vector2f(U1, V0), Vector4f(1, 1, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 0) + BlockPos, Vector2f(U0, V1), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 1, 1) + BlockPos, Vector2f(U1, V1), Vector4f(0, 1, 0, 0)));
    GenerateLastIndices();
}

void Chunk::GenBottomSide(Vector3Int BlockPos)
{
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 0) + BlockPos, Vector2f(U0, V0), Vector4f(1, 0, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(0, 0, 1) + BlockPos, Vector2f(U1, V0), Vector4f(0, 1, 0, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 0) + BlockPos, Vector2f(U0, V1), Vector4f(0, 0, 1, 0)));
    Vertices.push_back(PositionUVColorVertex(Vector3f(1, 0, 1) + BlockPos, Vector2f(U1, V1), Vector4f(1, 1, 1, 0)));
    GenerateLastIndices();
}
