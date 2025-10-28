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

		c_Softaim(std::weak_ptr<ui::c_render> _Renderer, std::weak_ptr<driver::c_Memory> _Memory)
		{
			if (!(Renderer = _Renderer.lock()))
				return;

			if (!(Memory = _Memory.lock()))
				return;

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

			if (Target.EntityID < 0)
				return;

			// todo
			if (!Target.isVisible)
				return;
			
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
			Mouse.MoveRelative(dx, dy);
		}

	};
}