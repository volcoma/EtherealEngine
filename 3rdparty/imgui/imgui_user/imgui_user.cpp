#include "imgui_user.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include <unordered_map>
#include <vector>
#include <array>
#include <cstring>
namespace ImGui
{

bool ImageButtonWithAspectAndTextDOWN(ImTextureID texId, const std::string& name, const ImVec2& texture_size, const ImVec2& imageSize,
									   const ImVec2& uv0, const ImVec2& uv1, int frame_padding,
									   const ImVec4& bg_col, const ImVec4& tint_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return false;

	ImVec2 size = imageSize;
	if(size.x <= 0 && size.y <= 0)
	{
		size.x = size.y = ImGui::GetTextLineHeightWithSpacing();
	}
	else
	{
		if(size.x <= 0)
			size.x = size.y;
		else if(size.y <= 0)
			size.y = size.x;
		size *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
	}

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

    
    ImVec2 size_name = ImGui::CalcTextSize(name.c_str(), nullptr, true);
    auto name_sz = name.size();
    std::string label_str = name;

	if(size_name.x > imageSize.x)
	{
		for(auto ds = name_sz; ds > 3; --ds)
		{
			size_name = ImGui::CalcTextSize(label_str.c_str(), nullptr, true);
			if(size_name.x < imageSize.x)
			{
				label_str[ds - 2] = '.';
				label_str[ds - 1] = '.';
				label_str[ds] = '.';
				break;
			}

			label_str.pop_back();
		}
	}
    
    const char* label = label_str.c_str();
    
	const ImGuiID id = window->GetID(label);
	const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
	const bool hasText = textSize.x > 0;

	const float innerSpacing =
		hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
	const ImVec2 padding =
		(frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
	bool istextBig = false;
	if(textSize.x > imageSize.x)
	{
		istextBig = true;
	}
	const ImVec2 totalSizeWithoutPadding(size.x, size.y > textSize.y ? size.y : textSize.y);

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
	ImVec2 start(0, 0);
	start = window->DC.CursorPos + padding;
	if(size.y < textSize.y)
	{
		start.y += (textSize.y - size.y) * .5f;
	}
	ImVec2 reajustMIN(0, 0);
	ImVec2 reajustMAX = size;
	if(bb.Max.y - textSize.y < start.y + reajustMAX.y)
	{
		reajustMIN.x += textSize.y / 2;
		reajustMAX.x -= textSize.y / 2;
		reajustMAX.y -= textSize.y;
	}
	ImRect image_bb(start + reajustMIN, start + reajustMAX);
	start = window->DC.CursorPos + padding;
	start.y += (size.y - textSize.y) + 2;
	if(istextBig == false)
	{
		start.x += (size.x - textSize.x) * .5f;
	}

	ItemSize(bb);
	if(!ItemAdd(bb, id))
		return false;

	bool hovered = false, held = false;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive
													: hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderFrame(bb.Min, bb.Max, col, true,
				ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
	if(bg_col.w > 0.0f)
		window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

	float w = texture_size.x;
	float h = texture_size.y;
	ImVec2 imgSz = ImVec2(image_bb.GetWidth(), image_bb.GetHeight());
	float max_size = ImMax(imgSz.x, imgSz.y);
	float aspect = w / h;
	if(w > h)
	{
		float m = ImMin(max_size, w);

		imgSz.x = m;
		imgSz.y = m / aspect;
	}
	else if(h > w)
	{
		float m = ImMin(max_size, h);

		imgSz.x = m * aspect;
		imgSz.y = m;
	}

	if(imgSz.x > imgSz.y)
		image_bb.Min.y += (max_size - imgSz.y) * 0.5f;
	if(imgSz.x < imgSz.y)
		image_bb.Min.x += (max_size - imgSz.x) * 0.5f;

    image_bb.Max = image_bb.Min + imgSz;
	window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

	if(textSize.x > 0)
	{
		ImGui::RenderText(start, label);
	}
	return pressed;
}

void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness)
{
	ImGuiWindow* window = GetCurrentWindow();

	if(border)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1),
								  GetColorU32(ImGuiCol_BorderShadow), rounding, 15, thickness);
		window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 15, thickness);
	}
}
static void PushMultiItemsWidthsAndLabels(const char* labels[], int components, float w_full)
{
	ImGuiWindow* window = GetCurrentWindow();
	const ImGuiStyle& style = GImGui->Style;
	if(w_full <= 0.0f)
		w_full = GetContentRegionAvailWidth();

	const float w_item_one =
		ImMax(1.0f, (w_full - (style.ItemInnerSpacing.x * 2.0f) * (components - 1)) / (float)components) -
		style.ItemInnerSpacing.x;
	for(int i = 0; i < components; i++)
		window->DC.ItemWidthStack.push_back(w_item_one - CalcTextSize(labels[i]).x);
	window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

bool DragFloatNEx(const char* labels[], float* v, int components, float v_speed, float v_min, float v_max,
				  const char* display_format, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	BeginGroup();

	PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
	for(int i = 0; i < components; i++)
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
bool DragUIntNEx(const char* labels[], unsigned int* v, int components, float v_speed, unsigned int v_min,
				 unsigned int v_max, const char* display_format)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	BeginGroup();

	PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
	for(int i = 0; i < components; i++)
	{
		PushID(labels[i]);
		PushID(i);
		TextUnformatted(labels[i], FindRenderedTextEnd(labels[i]));
		SameLine();
		int val = static_cast<int>(v[i]);
		value_changed |=
			DragInt("", &val, v_speed, static_cast<int>(v_min), static_cast<int>(v_max), display_format);
		v[i] = val;
		SameLine(0, g.Style.ItemInnerSpacing.x);
		PopID();
		PopID();
		PopItemWidth();
	}

	EndGroup();

	return value_changed;
}

bool DragIntNEx(const char* labels[], int* v, int components, float v_speed, int v_min, int v_max,
				const char* display_format)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	BeginGroup();

	PushMultiItemsWidthsAndLabels(labels, components, 0.0f);
	for(int i = 0; i < components; i++)
	{
		PushID(labels[i]);
		PushID(i);
		TextUnformatted(labels[i], FindRenderedTextEnd(labels[i]));
		SameLine();
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

	if(!enabled)
		Image(texture, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	else
	{
		if(ImageButton(texture, size, ImVec2(0, 0), ImVec2(1, 1), -1, bg_color))
		{
			ret = true;
		}
	}
	if(tooltip && IsItemHovered())
	{
		SetTooltip("%s", tooltip);
	}

	ImVec2 rectMin = GetItemRectMin();
	ImVec2 rectMax = GetItemRectMax();
	// ImVec2 rectSize = GetItemRectSize();
	// const float textHeight = GetTextLineHeight();

	if(selected)
	{
		PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
		RenderFrameEx(rectMin, rectMax, true, 0.0f, 2.0f);
		PopStyleColor();
	}

	PopStyleColor(3);
	PopStyleVar(3);
	return ret;
}

void ImageWithAspect(ImTextureID texture, ImVec2 texture_size, ImVec2 size, const ImVec2& uv0,
					 const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	float w = texture_size.x;
	float h = texture_size.y;
	float max_size = ImMax(size.x, size.y);
	float aspect = w / h;
	if(w > h)
	{
		float m = ImMin(max_size, w);

		size.x = m;
		size.y = m / aspect;
	}
	else if(h > w)
	{
		float m = ImMin(max_size, h);

		size.x = m * aspect;
		size.y = m;
	}

	auto pos = GetCursorScreenPos();
	ImGuiWindow* window = GetCurrentWindow();

	ImRect bb(window->DC.CursorPos,
			  ImVec2(window->DC.CursorPos.x + max_size, window->DC.CursorPos.y + max_size));
	ItemSize(bb);
	ItemAdd(bb, 0);

	auto pos2 = GetCursorScreenPos();

	if(size.x > size.y)
		pos.y += (max_size - size.y) * 0.5f;
	if(size.x < size.y)
		pos.x += (max_size - size.x) * 0.5f;

	SetCursorScreenPos(pos);

	Image(texture, size, uv0, uv1, tint_col, border_col);

	SetCursorScreenPos(pos2);
}

int ImageButtonWithAspectAndLabel(ImTextureID texture, ImVec2 texture_size, ImVec2 size, ImVec2 uv0,
								  ImVec2 uv1, bool selected, bool* edit_label, const char* label, char* buf,
								  size_t buf_size, ImGuiInputTextFlags flags)
{
	static bool edit = false;

	if(edit_label != nullptr)
		edit = *edit_label;

	int return_value = 0;

	ImGuiWindow* window = GetCurrentWindow();
	static bool inputActive = false;
	bool label_clicked = false;
	const float image_padding = 0.0f; // size.x * 0.3f;
	BeginGroup();
	{
		//		Dummy(ImVec2(image_padding, 0.0f));
		//        SameLine();
		if(selected)
		{
			ImageWithAspect(texture, texture_size, size, uv0, uv1, {0.7f, 0.7f, 0.7f, 1.0f});
			RenderFrameEx(GetItemRectMin(), GetItemRectMax(), true, 0.0f, 2.0f);
		}
		else
		{
			ImageWithAspect(texture, texture_size, size, uv0, uv1);
		}
		//        SameLine();
		//        Dummy(ImVec2(image_padding, 0.0f));

		auto pos = GetCursorPos();

		if(!(selected && edit))
		{
			float wrap_pos =
				pos.x + size.x +
				image_padding * 2.0f; // CalcWrapWidthForPos(pos, pos.x + size.x + image_padding * 2.0f);
			PushTextWrapPos(wrap_pos);
			AlignTextToFramePadding();
			TextUnformatted(label);

			PopTextWrapPos();
		}

		label_clicked = IsItemClicked(0);
		if(!edit)
		{
			auto& g = *GetCurrentContext();
			if(!g.DragDropActive && IsItemHovered())
			{
				BeginTooltip();
				TextUnformatted(label);
				EndTooltip();
			}
		}

		if(selected && edit)
		{
			SetCursorPos(pos);
			PushItemWidth(size.x + image_padding * 2.0f);

			if(!inputActive)
			{
				SetKeyboardFocusHere();
			}

			if(InputText("", &buf[0], buf_size, flags))
			{
				edit = false;
				return_value = 2;
			}

			PopItemWidth();
			inputActive = IsItemActive();
			if(!inputActive && (IsMouseClicked(0) || IsMouseDragging()))
			{
				edit = false;
			}
		}
	}
	EndGroup();

	static ImGuiID id = 0;

	if(IsItemHovered() && !IsMouseDragging(0))
	{
		if(IsMouseClicked(0))
		{
			id = window->GetID(label);

			if(!inputActive)
				SetActiveID(id, window);
		}

		if(!IsMouseDragging(0) && IsMouseReleased(0) && window->GetID(label) == id)
		{
			if(!selected)
				edit = false;

			return_value = 1;
		}

		if(selected && label_clicked)
		{
			edit = selected;
		}

		if(IsMouseDoubleClicked(0))
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
	// float padding = frame_padding.y * 2;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
							 ImGuiWindowFlags_NoSavedSettings;
	ImGui::SetNextWindowSize(size);
	bool ret = ImGui::Begin(str_id, nullptr, flags);
	ImGui::PopStyleVar(3);

	return ret;
}

void EndToolbar()
{
	auto height = ImGui::GetWindowHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImGui::End();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height);
}

void LabelTextEx(const char* label, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LabelTextExV(label, fmt, args);
	va_end(args);
} // Add a label+text combo aligned to other label+value widgets

void LabelTextExV(const char* label, const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, nullptr, true);
	const ImRect value_bb(window->DC.CursorPos,
						  window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
	const ImRect total_bb(
		window->DC.CursorPos,
		window->DC.CursorPos +
			ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) +
			label_size);
	ItemSize(total_bb, style.FramePadding.y);
	if(!ItemAdd(total_bb, 0))
		return;

	// Render
	const char* value_text_begin = &g.TempBuffer[0];
	const char* value_text_end =
		value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, nullptr,
					  ImVec2(0.0f, 0.5f));
	if(label_size.x > 0.0f)
		RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y),
				   label);
}

std::unordered_map<std::string, ImFont*>& GetFonts()
{
	static std::unordered_map<std::string, ImFont*> s_fonts;

	return s_fonts;
}

ImFont* GetFont(const std::string& id)
{
	auto& fonts = GetFonts();
	auto it = fonts.find(id);
	if(it != fonts.end())
		return it->second;

	return nullptr;
}

void AddFont(const std::string& id, ImFont* font)
{
	auto& fonts = GetFonts();
	fonts[id] = font;
}

void ClearFonts()
{
	auto& fonts = GetFonts();
	fonts.clear();
}

void PushFont(const std::string& id)
{
	PushFont(GetFont(id));
}
}
