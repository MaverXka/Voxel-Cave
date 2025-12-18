#pragma once

#include <DirectXMath.h>
#include "CoreTypes.h"

using namespace DirectX;


class Utils
{
public:

	static XMVECTOR FromD3DVector(Vector3f Vector)
	{
		return XMVectorSet(Vector.X, Vector.Y, Vector.Z, 1);
	}

};