#pragma once

#include "Log/Log.h"

#define ENGINE_ASSERT(x, ...) 
//{ if(!(x)) { LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#include "glm/glm.hpp"

using float2 = glm::vec2;
using float3 = glm::vec3;
using float4 = glm::vec4;

using uint2 = glm::uvec2;

using int2 = glm::ivec2;

#define ENUM_CLASS_OPERATORS(e) \
	inline e operator& (e a, e b) { return static_cast<e>(static_cast<int>(a) & static_cast<int>(b)); } \
	inline e& operator&= (e& a, e b) { a = a & b; return a; }; \
	inline e operator| (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); } \
	inline e& operator|= (e& a, e b) { a = a | b; return a; }; \
	inline e operator~ (e a) { return static_cast<e>(~static_cast<int>(a));} \
	inline bool isSet(e val, e flag) { return (val & flag) != static_cast<e>(0); } \
	inline void flipBit(e& val, e flag) { val = isSet(val, flag) ? (val & (~flag)) : (val | flag); }

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
#ifdef _WIN64
	/* Windows x64  */
#define PLATFORM_WINDOWS
#else
	/* Windows x86 */
#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define HZ_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define HZ_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
 /* We also have to check __ANDROID__ before __linux__
  * since android is based on the linux kernel
  * it has __linux__ defined */
#elif defined(__ANDROID__)
#define HZ_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define PLATFORM_LINUX
//#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection
