#pragma once

#include <algorithm>
#include "Core/CoreTypes.h"

struct AABB
{
	Vector3f Min;
	Vector3f Max;

    Vector2f IntersectRayAABB(Vector3f Origin, Vector3f RayDir)
    {
        Vector3f tMin = (Min - Origin) / RayDir;
        Vector3f tMax = (Max - Origin) / RayDir;

        Vector3f t1(
            (std::min)(tMin.X, tMax.X),
            (std::min)(tMin.Y, tMax.Y),
            (std::min)(tMin.Z, tMax.Z)
        );

        Vector3f t2(
            (std::max)(tMin.X, tMax.X),
            (std::max)(tMin.Y, tMax.Y),
            (std::max)(tMin.Z, tMax.Z)
        );

        float tNear = (std::max)((std::max)(t1.X, t1.Y), t1.Z);
        float tFar = (std::min)((std::min)(t2.X, t2.Y), t2.Z);
        return Vector2f(tNear, tFar);
    }

	bool IntersectOtherAABB(AABB& Other)
	{
		return (Min.X <= Other.Max.X && Max.X >= Other.Min.X) &&
			(Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y) &&
			(Min.Z <= Other.Max.Z && Max.Z >= Other.Min.Z);
	}

    bool IntersectOtherAABBV2(const AABB& Other) const
    {
        return (Min.X < Other.Max.X && Max.X > Other.Min.X) &&
            (Min.Y < Other.Max.Y && Max.Y > Other.Min.Y) &&
            (Min.Z < Other.Max.Z && Max.Z > Other.Min.Z);
    }

    Vector3f GetCenter()
    {
        return (Min + Max) * 0.5f;
    }
    Vector3f GetExtent()
    {
        return (Max - Min) * 0.5f;
    }

	//bool IntersectPoint(Vector3f Point)
	//{

	//}
};