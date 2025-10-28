#pragma once

namespace features 
{
	class c_ESP
	{
	private:
        std::shared_ptr<ui::c_render> Renderer = nullptr;
		std::shared_ptr<driver::c_Memory> Memory = nullptr;

		inline auto DrawCornerBox(float X, float Y, float W, float H, const ImColor color, float thickness) -> void
		{
			float lineW = (W / 3);
			float lineH = (H / 3);

			Renderer->Line(X, Y, X, Y + lineH, color, thickness);
			Renderer->Line(X, Y, X + lineW, Y, color, thickness);
			Renderer->Line(X + W - lineW, Y, X + W, Y, color, thickness);
			Renderer->Line(X + W, Y, X + W, Y + lineH, color, thickness);
			Renderer->Line(X, Y + H - lineH, X, Y + H, color, thickness);
			Renderer->Line(X, Y + H, X + lineW, Y + H, color, thickness);
			Renderer->Line(X + W - lineW, Y + H, X + W, Y + H, color, thickness);
			Renderer->Line(X + W, Y + H - lineH, X + W, Y + H, color, thickness);
		}

	public:
		
        bool isInitialized = false;

		c_ESP(std::shared_ptr<ui::c_render> _Rendering, std::shared_ptr<driver::c_Memory> _Memory)
		{
			Renderer = _Rendering;
			Memory = _Memory;

			isInitialized = true;
		}

		~c_ESP()
		{
			isInitialized = false;
		}

		// Runs in render thread
		inline auto onRender() -> void
		{
			if (!this->isInitialized)
				return;

			for (int i = 0; i <= PtrCache::Entities.Size(); i++)
			{
				Engine::Entity Target{};
				if (!PtrCache::Entities.Get(i, Target))
					continue;
				
				Vector2 Head2D = Target.HeadBonePos2D;
				Vector2 Bottom2D = Target.RootBonePos2D;

				// draw the box
				if (useESP && useBoxes)
				{
					float BoxHeight = (float)(Head2D.y - Bottom2D.y);
					float CornerHeight = (float)abs(Head2D.y - Bottom2D.y);
					float CornerWidth = (float)BoxHeight * 0.80f;

					DrawCornerBox(
						(float)Head2D.x - (CornerWidth * 0.5f), (float)Head2D.y,
						(float)CornerWidth, (float)CornerHeight,
						IM_COL32(boxColor[0], boxColor[1], boxColor[2], boxColor[3]), 4.0f
					);
				}

				// draw a line to each player
				if (useESP && useLines)
				{
					Renderer->Line(
						(float)CenterScreen.x, (float)Height * lineHeight,
						(float)Head2D.x, (float)Head2D.y,
						IM_COL32(lineColor[0], lineColor[1], lineColor[2], lineColor[3]), 1.0f
					);
				}

				if (useESP)
				{
					std::string_view Text = std::format("Player {0} : {1:.1f}m", Target.EntityID, Target.WorldDist * 0.01);
					Renderer->Text(
						ImVec2(Head2D.x, Head2D.y),
						ImColor(255, 255, 255, 255), 1, Text,
						Renderer->GetFonts().m_esp
					);
				}
			}

			PtrCache::Entities.Clear();
		}

		// Runs in main loop
		inline auto onUpdate() -> void
		{
			if (!this->isInitialized)
				return;

			// check if not ina loading screen
			uintptr_t GameState = Memory->Read<uintptr_t>(PtrCache::Gworld + Offsets::GameState);

			if (!GameState)
				return;

			// get a pointer to the array of players in the game
			uintptr_t PlayerArray = Memory->Read<uintptr_t>(GameState + Offsets::PlayerArray);

			// get how much players are in the game
			int PlayerList = Memory->ReadBuffer<int>(GameState + (Offsets::PlayerArray + sizeof(uintptr_t)));

			if (!PlayerList)
				sdk::boneCache.Clear();

			float ClosestDistance = FLT_MAX;

			Engine::Entity Target{};

			for (int i = 0; i < PlayerList; i++)
			{
				Target.PlayerState = Memory->Read<uintptr_t>(PlayerArray + (i * sizeof(uintptr_t)));

				if (!Target.PlayerState)
					continue;

				Target.Pawn = Memory->Read<uintptr_t>(Target.PlayerState + Offsets::PawnPrivate);

				// skip self
				if (!Target.Pawn || Target.Pawn == PtrCache::Player)
					continue;

				Target.Mesh = Memory->Read<uintptr_t>(Target.Pawn + Offsets::Mesh);

				if (!Target.Mesh)
					continue;

				// Positions
				Target.HeadBonePos3D = sdk::GetBoneWithRotation(Target.Mesh, selectedBone, i);
				Target.RootBonePos3D = sdk::GetBoneWithRotation(Target.Mesh, 0, -1);
				Target.HeadBonePos2D = sdk::ProjectWorldToScreen(Target.HeadBonePos3D, PtrCache::vCamera);
				Target.RootBonePos2D = sdk::ProjectWorldToScreen(Target.RootBonePos3D, PtrCache::vCamera);

				Vector2 Head2D = Target.HeadBonePos2D;
				Vector2 Bottom2D = Target.RootBonePos2D;

				// Data
				Target.CurrentWeapon = Memory->Read<uintptr_t>(Target.Pawn + Offsets::CurrentWeapon);
				Target.isDying = (Memory->Read<char>(Target.Pawn + Offsets::isDying) >> 4) & 1;
				Target.isDowned = (Memory->Read<char>(Target.Pawn + Offsets::isDBNO) >> 4) & 1;
				Target.isVisible = sdk::isVisible(Target.Mesh);
				Target.WorldDist = sdk::GetDistanceFromLocalPlayer(Target.RootBonePos3D);
				Target.ScreenDist = util::GetCrossDistance(Head2D.x, Head2D.y, CenterScreen.x, CenterScreen.y);
				Target.EntityID = i;

				double ScreenDist = Target.ScreenDist;

				// Get closest player
				if (ScreenDist < gotAimFOV && ScreenDist < ClosestDistance)
				{
					ClosestDistance = (float)ScreenDist;
					PtrCache::Target = Target;
				}

				PtrCache::Entities.Set(Target.EntityID, Target);
			}
		}
	};
}