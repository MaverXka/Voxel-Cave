#pragma once

#include <string>

class Resource
{
public:

	Resource();

	~Resource();

	void SetDebugResourceName(std::string inResourceName);

private:

	std::string ResourceName;

};
