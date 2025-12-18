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

	Chunk* MyChunk2 = new Chunk(Vector2Int(16, 0));
	Chunks.push_back(MyChunk2);
	MyChunk2->Generate();
    MyChunk2->GenerateChunkMesh();

	Chunk* MyChunk3 = new Chunk(Vector2Int(-16, 0));
	Chunks.push_back(MyChunk3);
	MyChunk3->Generate();
	MyChunk3->GenerateChunkMesh();
	
}

void World::Tick(float DeltaTime)
{
}
