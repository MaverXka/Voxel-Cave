#include "GameClient.h"
#include "Registry/Registry.h"

GameClient::GameClient()
{
}

void GameClient::ClientInitialization()
{
	Block* myblock = new Block();
	Registry::Register<Block>(Registries::Block, "Test", myblock);
}
