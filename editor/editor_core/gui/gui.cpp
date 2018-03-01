#include "gui.h"
#include <vector>
namespace gui
{
static std::vector<std::shared_ptr<void>> s_textures;
void Image(texture_info info, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */,
		   const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */,
		   const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */,
		   const ImVec4& _borderCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */)
{
	s_textures.push_back(info.texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if(info.texture && info.is_rt)
	{
		if(info.is_origin_bl)
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	ImGui::Image(info.texture.get(), _size, uv0, uv1, _tintCol, _borderCol);
}

bool ImageButton(texture_info info, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */,
				 const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */, int _framePadding /*= -1 */,
				 const ImVec4& _bgCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */,
				 const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */)
{
	s_textures.push_back(info.texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if(info.texture && info.is_rt)
	{
		if(info.is_origin_bl)
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	return ImGui::ImageButton(info.texture.get(), _size, uv0, uv1, _framePadding, _bgCol, _tintCol);
}

bool ImageButtonEx(texture_info info, const ImVec2& size, const char* tooltip, bool selected, bool enabled)
{
	s_textures.push_back(info.texture);
	return ImGui::ImageButtonEx(info.texture.get(), size, tooltip, selected, enabled);
}

void ImageWithAspect(texture_info info, const ImVec2& texture_size, const ImVec2& size, const ImVec2& _uv0,
					 const ImVec2& _uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	s_textures.push_back(info.texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;
	if(info.texture && info.is_rt)
	{
		if(info.is_origin_bl)
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}
	return ImGui::ImageWithAspect(info.texture.get(), texture_size, size, uv0, uv1, tint_col, border_col);
}

int ImageButtonWithAspectAndLabel(texture_info info, const ImVec2& texture_size, const ImVec2& size,
								  const ImVec2& _uv0, const ImVec2& _uv1, bool selected, bool* edit_label,
								  const char* label, char* buf, size_t buf_size,
								  ImGuiInputTextFlags flags /*= 0*/)
{
	s_textures.push_back(info.texture);
	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;
	if(info.texture && info.is_rt)
	{
		if(info.is_origin_bl)
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	return ImGui::ImageButtonWithAspectAndLabel(info.texture.get(), texture_size, size, uv0, uv1, selected,
												edit_label, label, buf, buf_size, flags);
}

void CleanupTextures()
{
	s_textures.clear();
}

bool ImageButtonWithAspectAndTextDOWN(texture_info info, const std::string& name, const ImVec2& texture_size,
									  const ImVec2& image_size, const ImVec2& _uv0, const ImVec2& _uv1,
									  int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	s_textures.push_back(info.texture);
	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;
	if(info.texture && info.is_rt)
	{
		if(info.is_origin_bl)
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	return ImGui::ImageButtonWithAspectAndTextDOWN(info.texture.get(), name, texture_size, image_size, uv0,
												   uv1, frame_padding, bg_col, tint_col);
}
}
