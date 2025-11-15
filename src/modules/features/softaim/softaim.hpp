#pragma once
#include "../../input/mouse.hpp"

namespace features
{
	class c_Softaim
	{
	private:
		std::shared_ptr<ui::c_render> Renderer = nullptr;
		std::shared_ptr<driver::c_Memory> Memory = nullptr;

		inline auto DrawFOVCircle(const ImColor color) -> void
		{
			Renderer->Circle(CenterScreen.x, CenterScreen.y, gotAimFOV, color, 128);
		}

	public:
		bool isInitialized = false;
		std::unique_ptr<MemoryInput::c_RotationInput> Mouse;

		// deprecated serial input method
		//inline static SerialInput::c_MouseSerial Mouse = SerialInput::c_MouseSerial(SerialConfig::ComPort, SerialConfig::BaudRate);

		c_Softaim(std::weak_ptr<ui::c_render> _Renderer, std::weak_ptr<driver::c_Memory> _Memory)
		{
			if (!(Renderer = _Renderer.lock()))
				return;

			if (!(Memory = _Memory.lock()))
				return;

			Mouse = std::make_unique<MemoryInput::c_RotationInput>(Memory);

			isInitialized = true;
		}

		~c_Softaim()
		{
			isInitialized = false;
		}

		inline auto onRender() -> void
		{
			if (!this->isInitialized)
				return;

			this->DrawFOVCircle(ImColor(fovColor[0], fovColor[1], fovColor[2], fovColor[3]));
		}

		inline auto onUpdate() -> void
		{
			if (!this->isInitialized)
				return;

			// check if aiming
			if (!GetAsyncKeyState(VK_RBUTTON))
				return;

			Engine::Entity Target = PtrCache::Target;

			if (Target.EntityIndex < 0)
				return;

			if (!Target.isVisible || Target.isDowned)
				return;

			Mouse->LookAtEntity(Target);
		}

		
	};
}