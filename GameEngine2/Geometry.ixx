export module Geometry;

import <format>;
import <iostream>;
import <assert.h>;

import "glm/glm.hpp";
import "glm/gtx/compatibility.hpp";


//template <typename T>
//class Vector2
//{
//public:
//	T x, y;
//
//	T operator[](int i) const
//	{
//		assert(i >= 0 && i <= 1);
//
//		if (i == 0)
//		{
//			return x;
//		}
//
//		return y;
//	}
//
//	T& operator[](int i)
//	{
//		assert(i >= 0 && i <= 1);
//
//		if (i == 0)
//		{
//			return x;
//		}
//
//		return y;
//	}
//
//	Vector2<T> operator+(const Vector2<T>& o) const
//	{
//		return Vector2(x + o.x, y + o.y);
//	}
//	Vector2<T>& operator+=(const Vector2<T>& o)
//	{
//		x += o.x;
//		y += o.y;
//		return *this;
//	}
//};

export class float2 : public glm::float2
{
public:
	float2() : glm::float2{} {}
	float2(float x, float y) : glm::float2{ x, y } {}

	void v()
	{
		x = 1;
	}
};

//export using float2 = glm::float2;
//export using int2 = Vector2<int>;

export namespace Geometry
{
	std::string hi();
	std::string hi()
	{
		return std::string();
	}
}