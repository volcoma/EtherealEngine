#include "gui_system.h"
#include "embedded/consolas.ttf.h"
#include "embedded/fs_ocornut_imgui.bin.h"
#include "embedded/roboto_regular.ttf.h"
#include "embedded/robotomono_regular.ttf.h"
#include "embedded/vs_ocornut_imgui.bin.h"

#include "runtime/assets/asset_manager.h"
#include "runtime/input/input.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/program.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/render_window.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/uniform.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/system/engine.h"
#include <unordered_map>

// -------------------------------------------------------------------

class UIContexts
{
public:
	void set_initial_context(ImGuiContext* context)
	{
		_initial_context = context;
	}

	void restore_initial_context()
	{
		if(_initial_context)
			gui::SetCurrentContext(_initial_context);
	}

private:
	ImGuiContext* _initial_context = nullptr;
};

static gui_style s_gui_style;
static UIContexts s_contexts;

static gfx::VertexDecl s_decl;
static std::unique_ptr<program> s_program;
static asset_handle<texture> s_font_texture;
static std::vector<std::shared_ptr<texture>> s_textures;
static std::unordered_map<std::string, ImFont*> s_fonts;

void renderFunc(ImDrawData* _drawData)
{
	// Render command lists
	for(int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
	{
		gfx::TransientVertexBuffer tvb;
		gfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = _drawData->CmdLists[ii];
		std::uint32_t numVertices = static_cast<std::uint32_t>(drawList->VtxBuffer.size());
		std::uint32_t numIndices = static_cast<std::uint32_t>(drawList->IdxBuffer.size());

		if(!(gfx::getAvailTransientVertexBuffer(numVertices, s_decl) == numVertices) ||
		   !(gfx::getAvailTransientIndexBuffer(numIndices) == numIndices))
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		gfx::allocTransientVertexBuffer(&tvb, numVertices, s_decl);
		gfx::allocTransientIndexBuffer(&tib, numIndices);

		ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(tvb.data);
		std::memcpy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(tib.data);
		std::memcpy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		std::uint32_t offset = 0;
		for(const ImDrawCmd *cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end();
			cmd != cmdEnd; ++cmd)
		{
			if(cmd->UserCallback)
			{
				cmd->UserCallback(drawList, cmd);
			}
			else if(0 != cmd->ElemCount)
			{
				std::uint64_t state =
					0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_MSAA |
					BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

				texture* tex = s_font_texture.get();
				program* prog = s_program.get();
				if(!prog)
					return;

				if(nullptr != cmd->TextureId)
				{
					tex = reinterpret_cast<texture*>(cmd->TextureId);
				}
				
                const std::uint16_t x = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
				const std::uint16_t y = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
				const std::uint16_t width = std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - x);
				const std::uint16_t height = std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - y);
                                
				gfx::setScissor(x, y, width, height);

				prog->set_texture(0, "s_tex", tex);

				gfx::setVertexBuffer(0, &tvb, 0, numVertices);
				gfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
				gfx::setState(state);
				gfx::submit(render_pass::get_pass(), prog->handle);
			}

			offset += cmd->ElemCount;
		}
	}
}

bool gui_system::initialize()
{
	s_contexts.set_initial_context(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.RenderDrawListsFn = renderFunc;

	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();

	switch(gfx::getRendererType())
	{
		case gfx::RendererType::Direct3D9:
			am.create_asset_from_memory<shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_dx9[0],
												sizeof(vs_ocornut_imgui_dx9));
			am.create_asset_from_memory<shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_dx9[0],
												sizeof(fs_ocornut_imgui_dx9));
			break;

		case gfx::RendererType::Direct3D11:
		case gfx::RendererType::Direct3D12:
			am.create_asset_from_memory<shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_dx11[0],
												sizeof(vs_ocornut_imgui_dx11));
			am.create_asset_from_memory<shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_dx11[0],
												sizeof(fs_ocornut_imgui_dx11));

			break;
		case gfx::RendererType::Metal:
			am.create_asset_from_memory<shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_mtl[0],
												sizeof(vs_ocornut_imgui_mtl));
			am.create_asset_from_memory<shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_mtl[0],
												sizeof(fs_ocornut_imgui_mtl));

			break;
		case gfx::RendererType::OpenGL:
		case gfx::RendererType::OpenGLES:
			am.create_asset_from_memory<shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_glsl[0],
												sizeof(vs_ocornut_imgui_glsl));
			am.create_asset_from_memory<shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_glsl[0],
												sizeof(fs_ocornut_imgui_glsl));

			break;
		default:

			break;
	}

	auto vs_ocornut_imgui = am.load<shader>("embedded:/vs_ocornut_imgui");
	auto fs_ocornut_imgui = am.load<shader>("embedded:/fs_ocornut_imgui");

	ts.push_or_execute_on_owner_thread(
		[](asset_handle<shader> vs, asset_handle<shader> fs) {
			s_program = std::make_unique<program>(vs, fs);

		},
		vs_ocornut_imgui, fs_ocornut_imgui);

	s_decl.begin()
		.add(gfx::Attrib::Position, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::TexCoord0, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::Color0, 4, gfx::AttribType::Uint8, true)
		.end();

	// init keyboard mapping
	io.KeyMap[ImGuiKey_Tab] = mml::keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = mml::keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = mml::keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = mml::keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = mml::keyboard::Down;
	io.KeyMap[ImGuiKey_Home] = mml::keyboard::Home;
	io.KeyMap[ImGuiKey_End] = mml::keyboard::End;
	io.KeyMap[ImGuiKey_Delete] = mml::keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = mml::keyboard::BackSpace;
	io.KeyMap[ImGuiKey_Enter] = mml::keyboard::Return;
	io.KeyMap[ImGuiKey_Escape] = mml::keyboard::Escape;
	io.KeyMap[ImGuiKey_A] = mml::keyboard::A;
	io.KeyMap[ImGuiKey_C] = mml::keyboard::C;
	io.KeyMap[ImGuiKey_V] = mml::keyboard::V;
	io.KeyMap[ImGuiKey_X] = mml::keyboard::X;
	io.KeyMap[ImGuiKey_Y] = mml::keyboard::Y;
	io.KeyMap[ImGuiKey_Z] = mml::keyboard::Z;

	std::uint8_t* data = nullptr;
	int width = 0;
	int height = 0;

	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	config.MergeMode = false;

	s_fonts["default"] = io.Fonts->AddFontDefault(&config);
	s_fonts["roboto_regular"] =
		io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), 17, &config);
	s_fonts["roboto_regular_mono"] = io.Fonts->AddFontFromMemoryTTF(
		(void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), 14.0f, &config);
	s_fonts["roboto_big"] =
		io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), 35, &config);
	s_fonts["consolas"] =
		io.Fonts->AddFontFromMemoryTTF((void*)s_consolas_embedded, sizeof(s_consolas_embedded), 20, &config);
	s_fonts["consolas_big"] =
		io.Fonts->AddFontFromMemoryTTF((void*)s_consolas_embedded, sizeof(s_consolas_embedded), 35, &config);

	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	s_font_texture = std::make_shared<texture>(
		static_cast<std::uint16_t>(width), static_cast<std::uint16_t>(height), false, 1,
		gfx::TextureFormat::BGRA8, 0, gfx::copy(data, static_cast<std::uint32_t>(width * height * 4)));

	// Store our identifier
	io.Fonts->SetTexID(s_font_texture.get());
	io.FontAllowUserScaling = true;
	s_gui_style.load_style();

	runtime::on_frame_begin.connect(this, &gui_system::frame_begin);

	return true;
}

void gui_system::dispose()
{
	runtime::on_frame_begin.disconnect(this, &gui_system::frame_begin);

	s_textures.clear();
	s_contexts.restore_initial_context();
	s_program.reset();
	s_font_texture.reset();

	s_fonts.clear();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = nullptr;
	ImGui::Shutdown();
}

void gui_system::frame_begin(std::chrono::duration<float>)
{
	s_textures.clear();
}

namespace gui
{
ImFont* GetFont(const std::string& id)
{
	auto it = s_fonts.find(id);
	if(it != s_fonts.end())
		return it->second;

	return nullptr;
}

void Image(std::shared_ptr<texture> texture, const ImVec2& _size,
		   const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */,
		   const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */,
		   const ImVec4& _borderCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */)
{
	s_textures.push_back(texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if(texture && texture->is_render_target())
	{
		if(gfx::is_origin_bottom_left())
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	ImGui::Image(texture.get(), _size, uv0, uv1, _tintCol, _borderCol);
}

bool ImageButton(std::shared_ptr<texture> texture, const ImVec2& _size,
				 const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */,
				 int _framePadding /*= -1 */, const ImVec4& _bgCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */,
				 const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */)
{
	s_textures.push_back(texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if(texture && texture->is_render_target())
	{
		if(gfx::is_origin_bottom_left())
		{
			uv0 = {0.0f, 1.0f};
			uv1 = {1.0f, 0.0f};
		}
	}

	return ImGui::ImageButton(texture.get(), _size, uv0, uv1, _framePadding, _bgCol, _tintCol);
}

bool ImageButtonEx(std::shared_ptr<texture> texture, const ImVec2& size, const char* tooltip, bool selected,
				   bool enabled)
{
	s_textures.push_back(texture);
	return ImGui::ImageButtonEx(texture.get(), size, tooltip, selected, enabled);
}

void ImageWithAspect(std::shared_ptr<texture> texture, const ImVec2& texture_size, const ImVec2& size,
					 const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	s_textures.push_back(texture);
	return ImGui::ImageWithAspect(texture.get(), texture_size, size, uv0, uv1, tint_col, border_col);
}

int ImageButtonWithAspectAndLabel(std::shared_ptr<texture> texture, const ImVec2& texture_size,
								  const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, bool selected,
								  bool* edit_label, const char* label, char* buf, size_t buf_size,
								  ImGuiInputTextFlags flags /*= 0*/)
{
	s_textures.push_back(texture);
	return ImGui::ImageButtonWithAspectAndLabel(texture.get(), texture_size, size, uv0, uv1, selected,
												edit_label, label, buf, buf_size, flags);
}

gui_style& get_gui_style()
{
	return s_gui_style;
}
}

void gui_style::reset_style()
{
	set_style_colors(hsv_setup());
}

void gui_style::set_style_colors(const hsv_setup& _setup)
{
	setup = _setup;
	ImVec4 col_text = ImColor::HSV(setup.col_text_hue, setup.col_text_sat, setup.col_text_val);
	ImVec4 col_main = ImColor::HSV(setup.col_main_hue, setup.col_main_sat, setup.col_main_val);
	ImVec4 col_back = ImColor::HSV(setup.col_back_hue, setup.col_back_sat, setup.col_back_val);
	ImVec4 col_area = ImColor::HSV(setup.col_area_hue, setup.col_area_sat, setup.col_area_val);
	float rounding = setup.frame_rounding;

	ImGuiStyle& style = gui::GetStyle();
	style.FrameRounding = rounding;
	style.WindowRounding = rounding;
	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.10f, 0.10f, 0.10f, 0.55f);
}

//////////////////////////////////////////////////////////////////////////
#include "../meta/interface/gui_system.hpp"
#include "core/serialization/associative_archive.h"

void gui_style::load_style()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor_data:/config/style.cfg");
	fs::error_code err;
	if(!fs::exists(absoluteKey, err))
	{
		save_style();
	}
	else
	{
		std::ifstream output(absoluteKey.string());
		cereal::iarchive_associative_t ar(output);

		try_load(ar, cereal::make_nvp("style", setup));
	}
}

void gui_style::save_style()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor_data:/config/style.cfg");
	std::ofstream output(absoluteKey.string());
	cereal::oarchive_associative_t ar(output);

	try_save(ar, cereal::make_nvp("style", setup));
}
