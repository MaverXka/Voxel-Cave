#include "Resource.h"
#include <d3d12.h>

Resource::Resource()
{
}

Resource::~Resource()
{
}

void Resource::SetDebugResourceName(std::string inResourceName)
{
	ResourceName = inResourceName;
}
