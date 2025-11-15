#pragma once

#include <serial/serial.h>
#include <format>
#include "../../memory/memory.hpp"
#include "../sdk/sdk.hpp"

namespace MemoryInput
{
	class c_RotationInput
	{
	private:
		std::shared_ptr<driver::c_Memory> Memory;

		auto WriteCoordinate(Vector3 newPos) -> void
		{
			Memory->Write(PtrCache::PlayerController + Offsets::RotationInput, newPos);
		}

	public:
		bool isInitialized = false;

		c_RotationInput(std::weak_ptr<driver::c_Memory> _Memory)
		{
			if (!(Memory = _Memory.lock()))
				return;

			isInitialized = true;
		}

		auto LookAtEntity(Engine::Entity Target) -> void
		{
			if (!isInitialized)
				return;

			Vector2 head = Target.HeadBonePos2D;
			if (!head.x || !head.y)
				return;

			if (!util::InsideBounds(head, Vector2(Width, Height)))
				return;

			const Vector2 delta{ 
				head.x - CenterScreen.x, 
				head.y - CenterScreen.y
			};

			const Vector2 smoothed{
				std::clamp(delta.x / gotAimSmoothness, -CenterScreen.x, CenterScreen.x),
				std::clamp(delta.y / gotAimSmoothness, -CenterScreen.y, CenterScreen.y)
			};

			const Vector3 rotationDelta{
				-smoothed.y / gotAimSmoothness, // pitch (invert)
				 smoothed.x / gotAimSmoothness, // yaw
				 0.0f
			};

			WriteCoordinate(rotationDelta);
		}
	};
}

namespace SerialInput
{
	class c_MouseSerial
	{
	private:
	public:
		enum Buttons
		{
			Left, Right
		};

		inline static std::unique_ptr<serial::Serial> Com;
		std::string_view Port;
		int BaudRate;

		c_MouseSerial(std::string_view _port, int _baudRate)
		{
			Port = _port;
			BaudRate = _baudRate;
			Com = std::make_unique<serial::Serial>(_port.data(), _baudRate, serial::Timeout::simpleTimeout(5));
		}

		~c_MouseSerial()
		{ 
			BaudRate = 0;
		}

		auto isConnected() -> bool
		{
			return Com->available();
		}

		auto MoveRelative(int x, int y) -> void
		{
			std::string _buffer = std::format("move {0} {1}", x, y);
			Com->write(_buffer + "\n");
		}

		auto Click(Buttons button) -> void
		{
			switch (button)
			{
			case 0:
				Com->write("lclick");
				break;
			case 1:
				Com->write("rclick");
				break;
			default:
				break;
			}
		}

		auto Press(Buttons button) -> void
		{
			switch (button)
			{
			case 0:
				Com->write("lpress");
				break;
			case 1:
				Com->write("rpress");
				break;
			default:
				break;
			}
		}

		auto Release(Buttons button) -> void
		{
			switch (button)
			{
			case 0:
				Com->write("lrelease");
				break;
			case 1:
				Com->write("rrelease");
				break;
			default:
				break;
			}
		}

		inline auto LookAtEntity(Engine::Entity Target) -> void
		{
			Vector2 targetPos = Target.HeadBonePos2D;

			// return if coord is outside of screen
			if (!util::InsideBounds(targetPos, Vector2(Width, Height)))
				return;

			static Vector2 currentPos = CenterScreen;

			// add some jitter
			Vector2 offset{
				static_cast<double>(rand() % 5),
				static_cast<double>(rand() % 5)
			};

			targetPos.x -= offset.x;
			targetPos.y -= offset.y;

			// smoothing factor -> smaller = smoother
			float smoothFactor = std::max(gotAimSmoothness * 0.05f, 0.05f);

			// linear interpolation
			currentPos.x += (targetPos.x - currentPos.x) * smoothFactor;
			currentPos.y += (targetPos.y - currentPos.y) * smoothFactor;

			// compute delta movement
			float dx = currentPos.x - CenterScreen.x;
			float dy = currentPos.y - CenterScreen.y;

			// tell arduino to move
			this->MoveRelative(dx, dy);
		}
	};
}