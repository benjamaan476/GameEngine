#pragma once

#include "Log/Log.h"

#define ENGINE_ASSERT(x, ...) if((!x)){ LOG_ERROR(__VA_ARGS__); __debugbreak();}
