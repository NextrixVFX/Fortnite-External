#pragma once

#include "esp/esp.hpp"
#include "softaim/softaim.hpp"

namespace features
{
	class c_Features
	{
	private:
		std::unique_ptr<features::c_ESP> Esp = nullptr;
		std::unique_ptr<features::c_Softaim> Softaim = nullptr;

	public:
		c_Features(std::shared_ptr<ui::c_render> _Rendering, std::shared_ptr<driver::c_Memory> _Memory)
		{
			Esp = std::make_unique<features::c_ESP>(_Rendering, _Memory);
			Softaim = std::make_unique<features::c_Softaim>(_Rendering, _Memory);
		}

		~c_Features()
		{
			Esp = nullptr;
			Softaim = nullptr;
		}

		inline auto onRender() -> void
		{
			// Render ESP
			if (Esp)
				Esp->onRender();

			// Render Softaim
			if (Softaim && useSoftaim)
				Softaim->onRender();
		}

		inline auto onUpdate() -> void
		{
			if (Esp && Esp->isInitialized)
				Esp->onUpdate();

			if (Softaim && Softaim->isInitialized && useSoftaim)
				Softaim->onUpdate();
		}
	};
}