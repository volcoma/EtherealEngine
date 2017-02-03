#include "gui_system.h"
#include "embedded/roboto_regular.ttf.h"
#include "embedded/robotomono_regular.ttf.h"
#include "embedded/vs_ocornut_imgui.bin.h"
#include "embedded/fs_ocornut_imgui.bin.h"

#include "runtime/rendering/vertex_buffer.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/program.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/uniform.h"
#include "runtime/rendering/render_window.h"
#include "runtime/assets/asset_manager.h"
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


static GUIStyle s_gui_style;
static UIContexts s_contexts;

static gfx::VertexDecl				s_decl;
static std::unique_ptr<Program>		s_program;
static AssetHandle<Texture>			s_font_texture;
static std::vector<std::shared_ptr<ITexture>> s_textures;


void renderFunc(ImDrawData *_drawData)
{
	// Render command lists
	for (int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
	{
		gfx::TransientVertexBuffer tvb;
		gfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = _drawData->CmdLists[ii];
		std::uint32_t numVertices = (std::uint32_t)drawList->VtxBuffer.size();
		std::uint32_t numIndices = (std::uint32_t)drawList->IdxBuffer.size();

		if (!gfx::getAvailTransientVertexBuffer(numVertices, s_decl) == numVertices
			|| !gfx::getAvailTransientIndexBuffer(numIndices) == numIndices)
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		gfx::allocTransientVertexBuffer(&tvb, numVertices, s_decl);
		gfx::allocTransientIndexBuffer(&tib, numIndices);

		ImDrawVert* verts = (ImDrawVert*)tvb.data;
		std::memcpy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = (ImDrawIdx*)tib.data;
		std::memcpy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		std::uint32_t offset = 0;
		for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
		{
			if (cmd->UserCallback)
			{
				cmd->UserCallback(drawList, cmd);
			}
			else if (0 != cmd->ElemCount)
			{
				std::uint64_t state = 0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
					| BGFX_STATE_MSAA
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
					;

				Texture* texture = s_font_texture.get();
				FrameBuffer* fbo = nullptr;
				Program* program = s_program.get();
				if (!program)
					return;

				if (nullptr != cmd->TextureId)
				{
					ITexture* bind = (ITexture*)cmd->TextureId;
					if (rttr::type::get(*bind) == rttr::type::get<Texture>())
						texture = (Texture*)bind;
					else if (rttr::type::get(*bind) == rttr::type::get<FrameBuffer>())
						fbo = (FrameBuffer*)bind;
				}

				const std::uint16_t xx = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
				const std::uint16_t yy = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
				gfx::setScissor(xx, yy
					, std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - xx)
					, std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - yy)
				);


				if (fbo)
					program->set_texture(0, "s_tex", fbo);
				else
					program->set_texture(0, "s_tex", texture);


				gfx::setVertexBuffer(&tvb, 0, numVertices);
				gfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
				gfx::setState(state);
				gfx::submit(RenderPass::get_pass(), program->handle);
			}

			offset += cmd->ElemCount;
		}
	}
}


bool GuiSystem::initialize()
{
	s_contexts.set_initial_context(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.RenderDrawListsFn = renderFunc;

	auto am = core::get_subsystem<runtime::AssetManager>();

	switch (gfx::getRendererType())
	{
	case gfx::RendererType::Direct3D9:
		am->create_asset_from_memory<Shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_dx9[0], sizeof(vs_ocornut_imgui_dx9));
		am->create_asset_from_memory<Shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_dx9[0], sizeof(fs_ocornut_imgui_dx9));
		break;

	case gfx::RendererType::Direct3D11:
	case gfx::RendererType::Direct3D12:
		am->create_asset_from_memory<Shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_dx11[0], sizeof(vs_ocornut_imgui_dx11));
		am->create_asset_from_memory<Shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_dx11[0], sizeof(fs_ocornut_imgui_dx11));

		break;
	case gfx::RendererType::Metal:
		am->create_asset_from_memory<Shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_mtl[0], sizeof(vs_ocornut_imgui_mtl));
		am->create_asset_from_memory<Shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_mtl[0], sizeof(fs_ocornut_imgui_mtl));

		break;
	case gfx::RendererType::OpenGL:
	case gfx::RendererType::OpenGLES:
		am->create_asset_from_memory<Shader>("embedded:/vs_ocornut_imgui", &vs_ocornut_imgui_glsl[0], sizeof(vs_ocornut_imgui_glsl));
		am->create_asset_from_memory<Shader>("embedded:/fs_ocornut_imgui", &fs_ocornut_imgui_glsl[0], sizeof(fs_ocornut_imgui_glsl));

		break;
	default:

		break;
	}
	am->load<Shader>("embedded:/vs_ocornut_imgui", false)
		.then([am](auto vs)
	{
		am->load<Shader>("embedded:/fs_ocornut_imgui", false)
			.then([vs](auto fs)
		{
			s_program = std::make_unique<Program>(vs, fs);
		});
	});

	s_decl
		.begin()
		.add(gfx::Attrib::Position, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::TexCoord0, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::Color0, 4, gfx::AttribType::Uint8, true)
		.end();

	// init keyboard mapping
	io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
	io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
	io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
	io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
	io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
	io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
	io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
	io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
	io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
	io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
	io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
	io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

	std::uint8_t* data = nullptr;
	std::int32_t width = 0;
	std::int32_t height = 0;

	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	config.MergeMode = false;
	config.MergeGlyphCenterV = true;

	io.Fonts->AddFontDefault(&config);
	io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), 17, &config);
	io.Fonts->AddFontFromMemoryTTF((void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), 14.0f, &config);
	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	s_font_texture = std::make_shared<Texture>(
		static_cast<std::uint16_t>(width)
		, static_cast<std::uint16_t>(height)
		, false
		, 1
		, gfx::TextureFormat::BGRA8
		, 0
		, gfx::copy(data, width*height * 4)
		);

	// Store our identifier
	io.Fonts->SetTexID(s_font_texture.get());

	s_gui_style.load_style();

	runtime::on_frame_begin.connect(this, &GuiSystem::frame_begin);

	return true;
}

void GuiSystem::dispose()
{
	runtime::on_frame_begin.disconnect(this, &GuiSystem::frame_begin);

	s_textures.clear();
	s_contexts.restore_initial_context();
	s_program.reset();
	s_font_texture.reset();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = nullptr;
	ImGui::Shutdown();
}

void GuiSystem::frame_begin(std::chrono::duration<float>)
{
	s_textures.clear();
}

namespace gui
{

void Image(std::shared_ptr<ITexture> texture, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */, const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */, const ImVec4& _borderCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */)
{
	s_textures.push_back(texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if (texture->is_render_target())
	{
		if (gfx::is_origin_bottom_left())
		{
			uv0 = { 0.0f, 1.0f };
			uv1 = { 1.0f, 0.0f };
		}
	}

	ImGui::Image(texture.get(), _size, uv0, uv1, _tintCol, _borderCol);
}

bool ImageButton(std::shared_ptr<ITexture> texture, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */, int _framePadding /*= -1 */, const ImVec4& _bgCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */, const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */)
{
	s_textures.push_back(texture);

	ImVec2 uv0 = _uv0;
	ImVec2 uv1 = _uv1;

	if (texture->is_render_target())
	{
		if (gfx::is_origin_bottom_left())
		{
			uv0 = { 0.0f, 1.0f };
			uv1 = { 1.0f, 0.0f };
		}
	}
	
	return ImGui::ImageButton(texture.get(), _size, uv0, uv1, _framePadding, _bgCol, _tintCol);
}

bool ImageButtonEx(std::shared_ptr<ITexture> texture, const ImVec2& size, const char* tooltip, bool selected, bool enabled)
{
	s_textures.push_back(texture);
	return ImGui::ImageButtonEx(texture.get(), size, tooltip, selected, enabled);
}

void ImageWithAspect(std::shared_ptr<ITexture> texture, const ImVec2& texture_size, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	s_textures.push_back(texture);
	return ImGui::ImageWithAspect(texture.get(), texture_size, size, uv0, uv1, tint_col, border_col);
}

int ImageButtonWithAspectAndLabel(std::shared_ptr<ITexture> texture, const ImVec2& texture_size, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags /*= 0*/)
{
	s_textures.push_back(texture);
	return ImGui::ImageButtonWithAspectAndLabel(texture.get(), texture_size, size, uv0, uv1, selected, edit_label, label, buf, buf_size, flags);
}

GUIStyle& get_gui_style()
{
	return s_gui_style;
}

}

void GUIStyle::reset_style()
{
	set_style_colors(HSVSetup());
}

void GUIStyle::set_style_colors(const HSVSetup& _setup)
{
	setup = _setup;
	ImVec4 col_text = ImColor::HSV(setup.col_text_hue, setup.col_text_sat, setup.col_text_val);
	ImVec4 col_main = ImColor::HSV(setup.col_main_hue, setup.col_main_sat, setup.col_main_val);
	ImVec4 col_back = ImColor::HSV(setup.col_back_hue, setup.col_back_sat, setup.col_back_val);
	ImVec4 col_area = ImColor::HSV(setup.col_area_hue, setup.col_area_sat, setup.col_area_val);
	float rounding = setup.frameRounding;

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
#include "core/serialization/archives.h"
#include "../meta/interface/gui_system.hpp"

void GUIStyle::load_style()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor_data:/config/style.cfg");
	if (!fs::exists(absoluteKey, std::error_code{}))
	{
		save_style();
	}
	else
	{
		std::ifstream output(absoluteKey);
		cereal::iarchive_json_t ar(output);

		try_load(ar, cereal::make_nvp("style", setup));
	}
}

void GUIStyle::save_style()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor_data:/config/style.cfg");
	std::ofstream output(absoluteKey);
	cereal::oarchive_json_t ar(output);

	try_save(ar, cereal::make_nvp("style", setup));
}
