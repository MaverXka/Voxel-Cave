#include "Config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Debug/Log/Logger.h"
using json = nlohmann::json;

Config::Config()
{
	std::ifstream f("Config.json");
	json data = json::parse(f);

	//data["WindowWidth"];
	Logger::Get()->LogFormat("Window width: {}", data["WindowWidth"].dump());
}

Config::~Config()
{
}