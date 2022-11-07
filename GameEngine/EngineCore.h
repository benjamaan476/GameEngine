#pragma once

#include "Log/Log.h"

#define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#include "glm/glm.hpp"

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;

using uint2 = glm::uvec2;