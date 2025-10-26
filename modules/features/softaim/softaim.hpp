#pragma once
#include "../../serial/mouse.hpp"

namespace features
{
	class c_Softaim
	{
	private:
		std::shared_ptr<ui::c_render> Renderer = nullptr;
		std::shared_ptr<driver::c_Memory> Memory = nullptr;

		inline auto DrawFOVCircle(const ImColor color) -> void
		{
			Renderer->Circle(CenterScreen.x, CenterScreen.y, gotAimFOV, color, 64);
		}

	public:
		bool isInitialized = false;
		inline static SerialInput::c_Mouse Mouse = SerialInput::c_Mouse(SerialConfig::ComPort, SerialConfig::BaudRate);

		c_Softaim()
		{ }

		~c_Softaim()
		{ }

		inline auto Initialize(std::shared_ptr<ui::c_render> Renderer_ptr, std::shared_ptr<driver::c_Memory> Memory_ptr) -> void
		{
			if (!(Renderer = Renderer_ptr))
				return;

			if (!(Memory = Memory_ptr))
				return;

			isInitialized = true;
		}

		inline auto onRender() -> void
		{
			this->DrawFOVCircle(ImColor(fovColor[0], fovColor[1], fovColor[2], fovColor[3]));
			this->onUpdate();
		}

		inline auto onUpdate() -> void
		{
			// check if aiming
			if (!GetAsyncKeyState(VK_RBUTTON))
				return;

			Engine::Entity* Target = &PtrCache::Target;

			if (!Target)
				return;

			// todo
			if (!Target->isVisible)
			{
				//std::cout << "Not Visible" << std::endl;
				//return;
			}

			std::cout << "ID:\t" << Target->EntityID << std::endl;

			Vector2 targetPos = Target->HeadBonePos2D;

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
			Mouse.MoveRelative(dx, dy);
		}

	};
}