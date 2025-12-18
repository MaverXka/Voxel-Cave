#include "World.h"
#include "Debug/Log/Logger.h"
#include "Chunk.h"
#include "Core/CoreTypes.h"

World::World()
{
	Start();
}


void World::Start()
{

	Logger::Get()->Log("World has been started.");

	Chunk* MyChunk = new Chunk(Vector2Int(0,0));
	Chunks.push_back(MyChunk);
	MyChunk->Generate();
	MyChunk->GenerateChunkMesh();

	Chunk* MyChunk2 = new Chunk(Vector2Int(1, 0));
	Chunks.push_back(MyChunk2);
	MyChunk2->Generate();
    MyChunk2->GenerateChunkMesh();

	Chunk* MyChunk3 = new Chunk(Vector2Int(-1, 0));
	Chunks.push_back(MyChunk3);
	MyChunk3->Generate();
	MyChunk3->GenerateChunkMesh();

	Chunk* MyChunk4 = new Chunk(Vector2Int(0, 1));
	Chunks.push_back(MyChunk4);
	MyChunk4->Generate();
	MyChunk4->GenerateChunkMesh();

	Chunk* MyChunk5 = new Chunk(Vector2Int(0, -1));
	Chunks.push_back(MyChunk5);
	MyChunk5->Generate();
	MyChunk5->GenerateChunkMesh();
	
}

void World::Tick(float DeltaTime)
{
}
