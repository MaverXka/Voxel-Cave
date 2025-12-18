#pragma once

#include <vector>
#include "Chunk.h"



class World
{
public:

	World();

	void Start();

	void Tick(float DeltaTime);

	bool IsWorldChunksEmpty() { return Chunks.empty(); }
	std::vector<Chunk*> GetWorldChunks() { return Chunks; }

private:

	std::vector<Chunk*> Chunks = std::vector<Chunk*>();

};
