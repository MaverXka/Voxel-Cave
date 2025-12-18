#pragma once

#include <iostream>

typedef unsigned char uint8;

template<typename NumberType>
struct Vector3
{
public:

	Vector3() : X(0), Y(0), Z(0)
	{

	}

	Vector3(NumberType NewX, NumberType NewY, NumberType NewZ) : X(NewX), Y(NewY), Z(NewZ)
	{

	}

	Vector3(NumberType NewVal) : X(NewVal), Y(NewVal), Z(NewVal)
	{

	}

	template<typename U>
	constexpr Vector3(const Vector3<U>& other)
		: X(static_cast<U>(other.X)),
		  Y(static_cast<U>(other.Y)),
		  Z(static_cast<U>(other.Z))
	{ }

	Vector3<NumberType> operator+ (const Vector3<NumberType>& First) const
	{
		return Vector3<NumberType>(X + First.X, Y + First.Y, Z + First.Z);
	}

	Vector3<NumberType> operator+= (Vector3<NumberType>& First)
	{
		return Vector3<NumberType>(X += First.X, Y += First.Y, Z += First.Z);
	}

	Vector3<NumberType> operator-= (Vector3<NumberType>& First)
	{
		return Vector3<NumberType>(X -= First.X, Y -= First.Y, Z -= First.Z);
	}

	Vector3<NumberType> operator* (const Vector3<NumberType>& First) const
	{
		return Vector3<NumberType>(X * First.X, Y * First.Y, Z * First.Z);
	}

	Vector3<NumberType> operator-( Vector3<NumberType>& First) const
	{
		return Vector3<NumberType>(X - First.X, Y - First.Y, Z - First.Z);
	}

	Vector3<NumberType> operator-(const Vector3<NumberType>& First) const
	{
		return Vector3<NumberType>(X - First.X, Y - First.Y, Z - First.Z);
	}


	Vector3<NumberType> operator/(Vector3<NumberType>& First) const
	{
		return Vector3<NumberType>(X / First.X, Y / First.Y, Z / First.Z);
	}

	bool operator>(const Vector3<NumberType>& Other) const
	{
		return X > Other.X & Y > Other.Y & Z > Other.Z;
	}


	bool operator<(const Vector3<NumberType>& Other) const
	{
		return X < Other.X & Y < Other.Y & Z < Other.Z;
	}


	Vector3<NumberType> operator* (NumberType First) const
	{
		return { X * First, Y * First, Z * First };
	}

	Vector3<NumberType> Normalized() const
	{
		NumberType len = static_cast<NumberType>(
			std::sqrt(static_cast<double>(X * X + Y * Y + Z * Z))
			);
		if (len == static_cast<NumberType>(0)) return { 0, 0, 0 };
		NumberType inv = static_cast<NumberType>(1) / len;
		return { X * inv, Y * inv, Z * inv };
	}

	static Vector3<NumberType> Cross(const Vector3<NumberType>& A, const Vector3<NumberType>& B)
	{
		return {
			A.Y*B.Z - A.Z*B.Y,
			A.Z*B.X - A.X*B.Z,
			A.X*B.Y - A.Y*B.X
		};
	}

	NumberType X;
	NumberType Y;
	NumberType Z;
};

template<typename NumberType>
struct Vector4
{
public:

	Vector4() : X(0), Y(0), Z(0), W(0)
	{

	}

	Vector4(NumberType NewX, NumberType NewY, NumberType NewZ, NumberType NewW) : X(NewX), Y(NewY), Z(NewZ), W(NewW)
	{

	}

	Vector4<NumberType> operator+ (const Vector4<NumberType>& First) const
	{
		return Vector4<NumberType>(X + First.X, Y + First.Y, Z + First.Z, W * First.W);
	}

	Vector4<NumberType> operator* (const Vector4<NumberType>& First) const
	{
		return Vector4<NumberType>(X * First.X, Y * First.Y, Z * First.Z, W * First.W);
	}


	Vector4<NumberType> operator* (NumberType First) const
	{
		return Vector4<NumberType>(X * First, Y * First, Z * First, W * First);
	}

	Vector4<NumberType> Normalized() const
	{
		auto len_sq = X * X + Y * Y + Z * Z + W * W;
		if (len_sq == static_cast<NumberType>(0))
			return { 0, 0, 0, 0 };

		auto len = static_cast<NumberType>(std::sqrt(static_cast<double>(len_sq)));
		auto inv = static_cast<NumberType>(1) / len;

		return { X * inv, Y * inv, Z * inv, W * inv };
	}

	NumberType X;
	NumberType Y;
	NumberType Z;
	NumberType W;
};

template<typename NumberType>
struct Vector2
{
public:

	Vector2() : X(0), Y(0)
	{

	}

	Vector2(NumberType NewX, NumberType NewY) : X(NewX), Y(NewY)
	{

	}

	Vector2<NumberType> operator+ (Vector2<NumberType>& First) const
	{
		return Vector2<NumberType>(X + First.X, Y + First.Y);
	}

	Vector2<NumberType> operator+ (const Vector2<NumberType>& First) const
	{
		return Vector2<NumberType>(X + First.X, Y + First.Y);
	}

	Vector2<NumberType> operator* (const Vector2<NumberType>& First) const
	{
		return Vector2<NumberType>(X * First.X, Y * First.Y);
	}

	Vector2<NumberType> operator* (const float& First) const
	{
		return Vector2<NumberType>(X * First, Y * First);
	}

	Vector2<NumberType> operator-(Vector2<NumberType>& First) const
	{
		return Vector2<NumberType>(X - First.X, Y - First.Y);
	}

	Vector2<NumberType> operator/(Vector2<NumberType>& First) const
	{
		return Vector2<NumberType>(X / First.X, Y / First.Y);
	}

	bool operator>(const Vector2<NumberType>& Other) const
	{
		return X > Other.X & Y > Other.Y;
	}


	bool operator<(const Vector2<NumberType>& Other) const
	{
		return X < Other.X & Y < Other.Y;
	}

	NumberType X;
	NumberType Y;
};


using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3Int = Vector3<int>;

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

using Vector2Int = Vector2<int>;

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;
