#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "setupapi.lib")

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#include "memory/memory.hpp"

#include <unordered_set>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <math.h>
#include <thread>

#include "modules/config.h"

#include "modules/sdk/engine.hpp"

#include "modules/sdk/sdk.hpp"

#include "modules/ui/render/render.hpp"

#include "modules/features/features.hpp"

#include "modules/ui/menu.hpp"