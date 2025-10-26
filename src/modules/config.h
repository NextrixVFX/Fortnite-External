#pragma once

// Softaim
inline static bool useSoftaim = true;
inline static float gotAimSmoothness = 7.0f;
inline static int gotAimFOV = 200;

// ESP
inline static bool useESP = false;
inline static bool useBoxes = true;
inline static bool useLines = true;
inline static float lineHeight = 0.0f;
inline static int selectedBone = 84;
inline static int gotFOV = 80; // espfov

// Customization
inline static float fovColor[4] = { 255.0f, 128.0f, 0.0f, 255.0f };
inline static float boxColor[4] = { 255.0f, 0.0f, 0.0f, 255.0f };
inline static float lineColor[4] = { 0.0f, 255.0f, 0.0f, 255.0f };

// Serial Communication
namespace SerialConfig
{
	inline static int BaudRate = 9600;
	inline static std::string_view ComPort = "COM9";
}