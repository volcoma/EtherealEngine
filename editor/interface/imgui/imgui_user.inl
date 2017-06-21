#include "imgui_user.h"
#include "imgui_internal.h"
#include <vector>
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

		auto frame_padding = GetStyle().FramePadding;
		PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

		bool ret = false;

		if (!enabled)
			Image(texture, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
		{
			if (ImageButton(texture, size, ImVec2(0, 0), ImVec2(1, 1), -1, bg_color))
			{
				ret = true;
			}
		}
		if (tooltip && IsItemHovered())
		{
			SetTooltip("%s", tooltip);
		}

		ImVec2 rectMin = GetItemRectMin();
		ImVec2 rectMax = GetItemRectMax();
		ImVec2 rectSize = GetItemRectSize();
		const float textHeight = GetTextLineHeight();
		
		if (selected)
		{
			PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
			RenderFrameEx(rectMin, rectMax, true, 0.0f, 2.0f);
			PopStyleColor();
		}


		
		PopStyleColor(3);
		PopStyleVar(3);
		return ret;
	}

	void ImageWithAspect(ImTextureID texture, ImVec2 texture_size, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		float w = texture_size.x;
		float h = texture_size.y;
		float max_size = ImMax(size.x, size.y);
		float aspect = w / h;
		if (w > h)
		{
			float m = ImMin(max_size, w);

			size.x = m;
			size.y = m / aspect;
		}
		else if (h > w)
		{
			float m = ImMin(max_size, h);

			size.x = m * aspect;
			size.y = m;
		}


		auto pos = GetCursorScreenPos();
		ImGuiWindow* window = GetCurrentWindow();

		ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + max_size, window->DC.CursorPos.y + max_size));
		ItemSize(bb);
		ItemAdd(bb, nullptr);

		auto pos2 = GetCursorScreenPos();

		if (size.x > size.y)
			pos.y += (max_size - size.y) * 0.5f;
		if (size.x < size.y)
			pos.x += (max_size - size.x) * 0.5f;

		SetCursorScreenPos(pos);

		Image(texture, size, uv0, uv1, tint_col, border_col);

		SetCursorScreenPos(pos2);
	}

	int ImageButtonWithAspectAndLabel(ImTextureID texture, ImVec2 texture_size, ImVec2 size, ImVec2 uv0, ImVec2 uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags)
	{
		static bool edit = false;

		if (edit_label != nullptr)
			edit = *edit_label;

		int return_value = 0;
		
		ImGuiWindow* window = GetCurrentWindow();
		bool inputActive = false;
		bool label_clicked = false;
		BeginGroup();
		{
			if(selected)
				ImageWithAspect(texture, texture_size, size, uv0, uv1, { 0.7f, 0.7f, 0.7f, 1.0f });
			else
				ImageWithAspect(texture, texture_size, size, uv0, uv1);
			if (selected)
				RenderFrameEx(GetItemRectMin(), GetItemRectMax(), true, 0.0f, 2.0f);

			auto pos = GetCursorScreenPos();
			PushItemWidth(size.x);
//			LabelTextEx("", label);
			if (!(selected && edit))
			{
				PushTextWrapPos(pos.x + size.x * 0.9f);
				AlignFirstTextHeightToWidgets();
				Text(label);
				PopTextWrapPos();
			}
			
			PopItemWidth();
			label_clicked = IsItemClicked(0);
			if (!edit)
			{
				if (IsItemHovered())
					SetTooltip(label);
			}

			if(selected && edit)
			{	
				SetCursorScreenPos(pos);
				PushItemWidth(size.x);

				if (InputText("",
				    &buf[0],
				    buf_size,
				    flags))
				{
					edit = false;
					return_value = 2;
				}

				PopItemWidth();
				inputActive = IsItemActive();
				if (!inputActive && (IsMouseClicked(0) || IsMouseDragging()))
				{
					edit = false;
				}
			
			}
		}
		EndGroup();

		
		static ImGuiID id = 0;

		if (IsItemHovered() && !IsMouseDragging(0))
		{
			if (IsMouseClicked(0))
			{
 				id = window->GetID(label);

				if(!inputActive)
 					SetActiveID(id);
			}

			if (!IsMouseDragging(0) && IsMouseReleased(0) && window->GetID(label) == id)
			{
				if (!selected)
					edit = false;

				return_value = 1;
			}

			if (selected && label_clicked)
			{
				edit = selected;
			}

			if (IsMouseDoubleClicked(0))
			{
				return_value = 3;
			}
		}
		

		return return_value;
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

	void LabelTextEx(const char* label, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		LabelTextExV(label, fmt, args);
		va_end(args);
	}// Add a label+text combo aligned to other label+value widgets

	void LabelTextExV(const char* label, const char* fmt, va_list args)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
		const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, NULL))
			return;

		// Render
		const char* value_text_begin = &g.TempBuffer[0];
		const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
		RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, ImGuiAlign_Center | ImGuiAlign_VCenter);
		if (label_size.x > 0.0f)
			RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
	}


	struct UserStacks
	{
		std::vector<ImVec2> max_label_sizes;
		std::vector<bool> label_on_the_left;
	};

	UserStacks& GetUserStacks()
	{
		static UserStacks stacks;
		return stacks;
	}

	void PushUserMaxLabelSize(const ImVec2& size)
	{
		auto& stacks = GetUserStacks();
		stacks.max_label_sizes.push_back(size);
	}
	void PopUserMaxLabelSize()
	{
		auto& stacks = GetUserStacks();
		stacks.max_label_sizes.pop_back();
	}

	void PushUserLabelIsLeft(bool left)
	{
		auto& stacks = GetUserStacks();
		stacks.label_on_the_left.push_back(left);
	}
	void PopUserLabelIsLeft()
	{
		auto& stacks = GetUserStacks();
		stacks.label_on_the_left.pop_back();
	}

	bool ComboBoxUser(const char* label, int* current_item, const char** items, int items_count, int height_in_items)
	{
		auto& stacks = GetUserStacks();
		bool label_on_the_left = false;
		
		if(!stacks.label_on_the_left.empty())
			label_on_the_left = stacks.label_on_the_left.front();

		if (label_on_the_left)
		{
			ImVec2 label_size = CalcTextSize(label);
			ImVec2 padding = { 0.0f, 0.0f };

			bool label_on_the_left = false;

			if (!stacks.max_label_sizes.empty())
			{
				ImVec2 max_label_size = stacks.max_label_sizes.front();

				padding = max_label_size - label_size;
			}
			
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			Dummy(padding);
			SameLine();
			PopStyleVar();
			TextUnformatted(label);
			SameLine();
			return Combo(("##" + std::string(label)).c_str(), current_item, items, items_count, height_in_items);
		}
		else
		{
			return Combo(label, current_item, items, items_count, height_in_items);
		}
	}

	bool InputTextUser(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
	{
		auto& stacks = GetUserStacks();
		bool label_on_the_left = false;

		if (!stacks.label_on_the_left.empty())
			label_on_the_left = stacks.label_on_the_left.front();

		if (label_on_the_left)
		{
			ImVec2 label_size = CalcTextSize(label);
			ImVec2 padding = { 0.0f, 0.0f };

			bool label_on_the_left = false;

			if (!stacks.max_label_sizes.empty())
			{
				ImVec2 max_label_size = stacks.max_label_sizes.front();

				padding = max_label_size - label_size;
			}

			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			Dummy(padding);
			SameLine();
			PopStyleVar();
			TextUnformatted(label);
			SameLine();
			return InputText(("##" + std::string(label)).c_str(), buf, buf_size, flags, callback, user_data);
		}
		else
		{
			return InputText(label, buf, buf_size, flags, callback, user_data);
		}
	}
}
