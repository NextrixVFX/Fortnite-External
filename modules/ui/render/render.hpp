#pragma once
#include <imgui/imgui.h>

namespace ui
{
	class c_render
	{
	private:
		enum eDrawFlags { drop_shadow, outline };

		ImDrawList* mDrawList { };

		struct fonts_t
		{
			ImFont* m_esp { };
		}
		mFonts{ };


	public:
		inline auto Initialize() -> void
		{
			ImGuiIO& io = ImGui::GetIO();
			mFonts.m_esp = io.Fonts->AddFontFromFileTTF(encrypt("C:\\Windows\\Fonts\\Verdana.ttf"), 16.0f);
			ImGui::GetIO().Fonts->AddFontDefault();
		}

		inline auto SetDrawList(ImDrawList* drawList) -> void
		{
			mDrawList = drawList;
		}

		inline auto Text(const ImVec2& txt_pos, ImU32 color, const int flags, const std::string_view& message, ImFont* font) const -> void
		{
			mDrawList->PushTextureID(font->ContainerAtlas->TexID);

			const auto size = font->CalcTextSizeA(font->LegacySize, FLT_MAX, 0.0f, message.data());
			const auto pos = ImVec2(txt_pos.x - size.x / 2.0f, txt_pos.y);
			auto outline_clr = IM_COL32(0, 0, 0, static_cast<int>(color * 0.3f));

			if (flags & drop_shadow)
				mDrawList->AddText(ImVec2(pos.x + 1, pos.y + 1), outline_clr, message.data());

			if (flags & outline)
			{
				constexpr std::array< ImVec2, 8 > offsets = {
					{
						{ 1, -1 }, { -1, 1 }, { -1, -1 }, { 1, 1 }, { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }
					}
				};

				for (const auto& offset : offsets)
					mDrawList->AddText(pos + offset, outline_clr, message.data());
			}

			mDrawList->AddText(pos, color, message.data());
		}

		inline auto Line(float x1, float y1, float x2, float y2, ImU32 clr, float thickness = 1.0f) const -> void
		{
			mDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), clr, thickness);
		}

		inline auto Rect(float x, float y, float w, float h, ImU32 clr, float rounding = 0.0f) const -> void
		{
			mDrawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), clr, rounding);
		}

		inline auto FilledRect(float x, float y, float w, float h, ImU32 color, float rounding, int flags) const -> void
		{
			mDrawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding, flags);
		}

		inline auto FilledRectGradient(float x, float y, float w, float h, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left) const -> void
		{
			mDrawList->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), col_upr_left, col_upr_right, col_bot_right, col_bot_left);
		}

		inline auto Triangle(float x1, float y1, float x2, float y2, float x3, float y3, ImU32 clr, float thickness) const -> void
		{
			mDrawList->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr, thickness);
		}

		inline auto TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, ImU32 clr) const -> void
		{
			mDrawList->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr);
		}

		inline auto Circle(float x1, float y1, float radius, ImU32 col, int segments) const -> void
		{
			mDrawList->AddCircle(ImVec2(x1, y1), radius, col, segments);
		}

		inline auto CircleFilled(float x1, float y1, float radius, ImU32 col, int segments) const -> void
		{
			mDrawList->AddCircleFilled(ImVec2(x1, y1), radius, col, segments);
		}


		inline auto& GetDrawList()
		{
			return mDrawList;
		}

		inline auto& GetFonts()
		{
			return mFonts;
		}
	};
}