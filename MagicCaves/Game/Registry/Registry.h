#pragma once

#include "pch.h"

enum class Registries
{
	Block,
	Item,
	BlockEntity,
	Entity,

};

#define REGISTRYLOG "Registry"

class Registry
{
public:

	template<typename T>
	static void Register(Registries registry, const char* ID, T* Game);

protected:


	std::vector<Block*> RegisteredBlocks;

};

template<typename T>
inline void Registry::Register(Registries registry, const char* ID, T* Game)
{
	switch (registry)
	{
	case Registries::Block:

		if (std::is_base_of<Block, T>::value == 1)
		{
			LOG(REGISTRYLOG, "Registered block");
		}

		break;
	case Registries::Item:
		break;
	case Registries::BlockEntity:
		break;
	case Registries::Entity:
		break;
	default:
		break;
	}
}
