#include "imgui_user.h"
#include "imgui_internal.h"

namespace ImGui
{
	void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (border)
		{
			window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, 15, thickness);
			window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 15, thickness);
		}
	}
	static void PushMultiItemsWidthsAndLabels(const char* labels[], int components, float w_full)
	{
		ImGuiWindow* window = GetCurrentWindow();
		const ImGuiStyle& style = GImGui->Style;
		if (w_full <= 0.0f)
			w_full = GetContentRegionAvailWidth();

		const float w_item_one = ImMax(1.0f, (w_full - (style.ItemInnerSpacing.x*2.0f) * (components - 1)) / (float)components) - style.ItemInnerSpacing.x;
		for (int i = 0; i < components; i++)
			window->DC.ItemWidthStack.push_back(w_item_one - CalcTextSize(labels[i]).x);
		window->DC.ItemWidth = window->DC.ItemWidthStack.back();
	}

	bool DragFloatNEx(const char* labels[], float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();
		
		PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
		for (int i = 0; i < components; i++)
		{
			PushID(labels[i]);
			PushID(i);
			TextUnformatted(labels[i], FindRenderedTextEnd(labels[i]));
			SameLine();
			value_changed |= DragFloat("", &v[i], v_speed, v_min, v_max, display_format, power);
			SameLine(0, g.Style.ItemInnerSpacing.x);
			PopID();
			PopID();
			PopItemWidth();
		}

		EndGroup();

		return value_changed;
	}
	bool DragUIntNEx(const char* labels[], unsigned int* v, int components, float v_speed, unsigned int v_min, unsigned int v_max, const char* display_format)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();

		PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
		for (int i = 0; i < components; i++)
		{
			PushID(labels[i]);
			PushID(i);
			TextUnformatted(labels[i], FindRenderedTextEnd(labels[i])); SameLine();
			int val = static_cast<int>(v[i]);
			value_changed |= DragInt("", &val, v_speed, static_cast<int>(v_min), static_cast<int>(v_max), display_format);
			v[i] = val;
			SameLine(0, g.Style.ItemInnerSpacing.x);
			PopID();
			PopID();
			PopItemWidth();
		}

		EndGroup();

		return value_changed;
	}

	bool DragIntNEx(const char* labels[], int* v, int components, float v_speed, int v_min, int v_max, const char* display_format)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();

		PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
		for (int i = 0; i < components; i++)
		{
			PushID(labels[i]);
			PushID(i);
			TextUnformatted(labels[i], FindRenderedTextEnd(labels[i])); SameLine();
			value_changed |= DragInt("", &v[i], v_speed, v_min, v_max, display_format);
			SameLine(0, g.Style.ItemInnerSpacing.x);
			PopID();
			PopID();
			PopItemWidth();
		}

		EndGroup();

		return value_changed;
	}

	bool ToolbarButton(ImTextureID texture, const char* tooltip, bool selected, bool enabled)
	{
		return ImageButtonEx(texture, ImVec2(24, 24), tooltip, selected, enabled);
	}

	bool ImageButtonEx(ImTextureID texture, ImVec2 size, const char* tooltip, bool selected, bool enabled)
	{
		
		ImVec4 bg_color(0, 0, 0, 0);
// 		if (selected)
// 		{
// 			bg_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
// 		}

		auto frame_padding = ImGui::GetStyle().FramePadding;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

		bool ret = false;

		if (!enabled)
			ImGui::Image(texture, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
		{
			if (ImGui::ImageButton(texture, size, ImVec2(0, 0), ImVec2(1, 1), -1, bg_color))
			{
				ret = true;
			}
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", tooltip);
		}

		ImVec2 rectMin = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		ImVec2 rectSize = ImGui::GetItemRectSize();
		const float textHeight = ImGui::GetTextLineHeight();
		
		if (selected)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
			ImGui::RenderFrameEx(rectMin, rectMax, true, 0.0f, 2.0f);
			ImGui::PopStyleColor();
		}


		
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(3);
		return ret;
	}

	bool BeginToolbar(const char* str_id, ImVec2 screen_pos, ImVec2 size)
	{
		ImGui::SetNextWindowPos(screen_pos);
		auto frame_padding = ImGui::GetStyle().FramePadding;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		float padding = frame_padding.y * 2;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
		ImGui::SetNextWindowSize(size);
		bool ret = ImGui::Begin(str_id, nullptr, size, -1, flags);
		ImGui::PopStyleVar(3);


		return ret;
	}


	void EndToolbar()
	{
		auto height = ImGui::GetWindowHeight() + ImGui::GetStyle().FramePadding.y*2.0f;
		ImGui::End();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height);

	}
}
