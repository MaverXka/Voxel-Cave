#pragma once

#include "nlohmann/json.hpp"

class Config
{
public:
	Config();
	~Config();

	unsigned int WindowWidth = 0;
	unsigned int WindowHeight = 0;


private:

};