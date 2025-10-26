#pragma once

#include "../modules/sdk/engine.hpp"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

inline static int Width = GetSystemMetrics(SM_CXSCREEN);
inline static int Height = GetSystemMetrics(SM_CYSCREEN);

inline static Vector2 CenterScreen = {
	Width * 0.5f, Height * 0.5f
};

namespace util
{
	inline bool InsideBounds(Vector2 Position, Vector2 Coords)
	{
		return ((Position.x < Coords.x && Position.x > 0) &&
				(Position.y < Coords.y && Position.y > 0));
	}

	inline double GetCrossDistance(double x1, double y1, double x2, double y2)
	{
		return std::sqrt(std::pow((x2 - x1), 2.0) + std::pow((y2 - y1), 2.0));
	}

	inline auto PrintV2(std::string_view header, Vector2 Data) -> void
	{
		std::cout << header << std::dec << Data.x << " " << Data.y;
	}

	inline auto PrintV3(std::string_view header, Vector3 Data) -> void
	{
		std::cout << header << std::dec << Data.x << " " << Data.y << " " << Data.z;
	}

	inline auto PrintV4(std::string_view header, Engine::FQuat Data) -> void
	{
		std::cout << header << std::dec << Data.x << " " << Data.y << " " << Data.z << " " << Data.w;
	}
}