#ifndef IMGUI_USER_H
#define IMGUI_USER_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui.h"
#include <string>
namespace ImGui
{
IMGUI_API bool ImageButtonWithAspectAndTextDOWN(ImTextureID texId, const std::string& name,
												 const ImVec2& texture_size, const ImVec2& imageSize,
												 const ImVec2& uv0 = ImVec2(0, 0),
												 const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1,
												 const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
												 const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

IMGUI_API ImFont* GetFont(const std::string& id);
IMGUI_API void AddFont(const std::string& id, ImFont* font);
IMGUI_API void PushFont(const std::string& id);
IMGUI_API void ClearFonts();
IMGUI_API bool DragFloatNEx(const char* labels[], float* v, int components, float v_speed = 1.0f,
							float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f",
							ImGuiSliderFlags flags = 0);
IMGUI_API bool DragIntNEx(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0,
						  int v_max = 0, const char* display_format = "%.0f", ImGuiSliderFlags flags = 0);
IMGUI_API bool DragUIntNEx(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f,
						   unsigned int v_min = 0, unsigned int v_max = 0,
						   const char* display_format = "%.0f", ImGuiSliderFlags flags = 0);
IMGUI_API void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness = 1.0f);
IMGUI_API bool BeginToolbar(const char* str_id, ImVec2 screen_pos, ImVec2 size);
IMGUI_API void EndToolbar();
IMGUI_API bool ToolbarButton(ImTextureID texture, const char* tooltip, bool selected = false,
							 bool enabled = true);
IMGUI_API bool ImageButtonEx(ImTextureID texture, ImVec2 size = ImVec2(24, 24), const char* tooltip = nullptr,
							 bool selected = false, bool enabled = true);
IMGUI_API int ImageButtonWithAspectAndLabel(ImTextureID texture, ImVec2 texture_size, ImVec2 size, ImVec2 uv0,
											ImVec2 uv1, bool selected, bool* edit_label, const char* label,
											char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);
IMGUI_API void ImageWithAspect(ImTextureID texture, ImVec2 texture_size, ImVec2 size,
							   const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
							   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
							   const ImVec4& border_col = ImVec4(0, 0, 0, 0));
IMGUI_API void LabelTextEx(const char* label, const char* fmt, ...);
IMGUI_API void LabelTextExV(const char* label, const char* fmt, va_list args);
}

enum ImGuiMouseCursorEx_
{
	ImGuiMouseCursor_Help = ImGuiMouseCursor_COUNT,
	ImGuiMouseCursor_Wait,
	ImGuiMouseCursor_ArrowWait,
	ImGuiMouseCursor_Cross,
};
#endif
