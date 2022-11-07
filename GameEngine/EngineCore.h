#pragma once

#include "Log/Log.h"

#define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#include "glm/glm.hpp"

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;

using uint2 = glm::uvec2;

#define ENUM_CLASS_OPERATORS(e) \
	inline e operator& (e a, e b) { return static_cast<e>(static_cast<int>(a) & static_cast<int>(b)); } \
	inline e& operator&= (e& a, e b) { a = a & b; return a; }; \
	inline e operator| (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); } \
	inline e& operator|= (e& a, e b) { a = a | b; return a; }; \
	inline e operator~ (e a) { return static_cast<e>(~static_cast<int>(a));} \
	inline bool isSet(e val, e flag) { return (val & flag) != static_cast<e>(0); } \
	inline void flipBit(e& val, e flag) { val = isSet(val, flag) ? (val & (~flag)) : (val | flag); }
