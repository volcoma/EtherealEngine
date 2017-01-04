#pragma  once
#include "imgui.h"

namespace ImGui
{
	IMGUI_API bool DragFloatNEx(const char* labels[], float* v, int components, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
	IMGUI_API bool DragIntNEx(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
	IMGUI_API bool DragUIntNEx(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* display_format = "%.0f");
	IMGUI_API void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness = 1.0f);
	IMGUI_API bool BeginToolbar(const char* str_id, ImVec2 screen_pos, ImVec2 size);
	IMGUI_API void EndToolbar();
	IMGUI_API bool ToolbarButton(ImTextureID texture, const char* tooltip, bool selected = false, bool enabled = true);
	IMGUI_API bool ImageButtonEx(ImTextureID texture, ImVec2 size = ImVec2(24, 24), const char* tooltip = nullptr, bool selected = false, bool enabled = true);
	IMGUI_API int ImageButtonWithLabel(ImTextureID texture, ImVec2 size, bool selected, const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);

}