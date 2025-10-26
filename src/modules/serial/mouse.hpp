#pragma once

#include <serial/serial.h>
#include <format>

namespace SerialInput
{
	class c_Mouse
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

		c_Mouse(std::string_view _port, int _baudRate)
		{
			Port = _port;
			BaudRate = _baudRate;
			Com = std::make_unique<serial::Serial>(_port.data(), _baudRate, serial::Timeout::simpleTimeout(5));
		}

		~c_Mouse()
		{ BaudRate = 0; }

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
	};
}