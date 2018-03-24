#include "gui_system.h"

#include "core/filesystem/filesystem.h"
#include "core/graphics/index_buffer.h"
#include "core/graphics/render_pass.h"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"
#include "core/graphics/uniform.h"
#include "core/graphics/vertex_buffer.h"
#include "core/logging/logging.h"
#include "core/system/subsystem.h"

#include "runtime/assets/asset_manager.h"
#include "runtime/input/input.h"
#include "runtime/rendering/gpu_program.h"
#include "runtime/rendering/render_window.h"
#include <unordered_map>

#include "editor_core/gui/embedded/editor_default.ttf.h"
#include "editor_core/gui/embedded/fontawesome_webfont.ttf.h"
#include "editor_core/gui/embedded/fs_ocornut_imgui.bin.h"
#include "editor_core/gui/embedded/vs_ocornut_imgui.bin.h"
#include "editor_core/gui/gui.h"
//////////////////////////////////////////////////////////////////////////
#include "../meta/interface/gui_system.hpp"
#include "core/serialization/associative_archive.h"
namespace
{
const gfx::embedded_shader s_embedded_shaders[] = {BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
												   BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
												   BGFX_EMBEDDED_SHADER_END()};
// -------------------------------------------------------------------
std::unique_ptr<gpu_program> s_program;
asset_handle<gfx::texture> s_font_texture;
std::uint32_t s_draw_calls = 0;

void render_func(ImDrawData* _draw_data)
{
	if(_draw_data == nullptr || s_program == nullptr)
	{
		return;
	}
	s_draw_calls = 0;
	auto program = s_program.get();
	program->begin();
	// Render command lists
	for(int32_t ii = 0, num = _draw_data->CmdListsCount; ii < num; ++ii)
	{
		gfx::transient_vertex_buffer tvb;
		gfx::transient_index_buffer tib;

		const ImDrawList* draw_list = _draw_data->CmdLists[ii];
		std::uint32_t num_vertices = static_cast<std::uint32_t>(draw_list->VtxBuffer.size());
		std::uint32_t num_indices = static_cast<std::uint32_t>(draw_list->IdxBuffer.size());

		const auto& layout = gfx::pos_texcoord0_color0_vertex::get_layout();

		if(!(gfx::get_avail_transient_vertex_buffer(num_vertices, layout) == num_vertices) ||
		   !(gfx::get_avail_transient_index_buffer(num_indices) == num_indices))
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		gfx::alloc_transient_vertex_buffer(&tvb, num_vertices, layout);
		gfx::alloc_transient_index_buffer(&tib, num_indices);

		ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(tvb.data);
		std::memcpy(verts, draw_list->VtxBuffer.begin(), num_vertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(tib.data);
		std::memcpy(indices, draw_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx));

		std::uint64_t state =
			0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
			BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

		std::uint32_t offset = 0;
		for(const ImDrawCmd *cmd = draw_list->CmdBuffer.begin(), *cmdEnd = draw_list->CmdBuffer.end();
			cmd != cmdEnd; ++cmd)
		{
			if(cmd->UserCallback != nullptr)
			{
				cmd->UserCallback(draw_list, cmd);
			}
			else if(0 != cmd->ElemCount)
			{
				auto tex = s_font_texture.get();

				if(nullptr != cmd->TextureId)
				{
					tex = reinterpret_cast<gfx::texture*>(cmd->TextureId);
				}

				const std::uint16_t x = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
				const std::uint16_t y = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
				const std::uint16_t width = std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - x);
				const std::uint16_t height = std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - y);

				program->set_texture(0, "s_tex", tex);

				gfx::set_scissor(x, y, width, height);
				gfx::set_state(state);
				gfx::set_vertex_buffer(0, &tvb, 0, num_vertices);
				gfx::set_index_buffer(&tib, offset, cmd->ElemCount);
				gfx::submit(gfx::render_pass::get_pass(), program->native_handle());
				s_draw_calls++;
			}

			offset += cmd->ElemCount;
		}
	}
	program->end();
}

void imgui_handle_event(const mml::platform_event& event)
{
	auto& io = gui::GetIO();
	if(event.type == mml::platform_event::lost_focus)
	{
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		io.MouseDown[2] = false;
	}

	if(event.type == mml::platform_event::key_pressed)
	{
		io.KeysDown[event.key.code] = true;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if(event.type == mml::platform_event::key_released)
	{
		io.KeysDown[event.key.code] = false;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if(event.type == mml::platform_event::mouse_wheel_scrolled)
	{
		io.MouseWheel += event.mouse_wheel_scroll.delta;
	}

	if(event.type == mml::platform_event::mouse_button_pressed)
	{
		io.MouseDown[event.mouse_button.button] = true;
	}

	if(event.type == mml::platform_event::mouse_button_released)
	{
		io.MouseDown[event.mouse_button.button] = false;
	}

	if(event.type == mml::platform_event::mouse_moved)
	{
		io.MousePos.x = float(event.mouse_move.x);
		io.MousePos.y = float(event.mouse_move.y);
	}

	if(event.type == mml::platform_event::text_entered)
	{
		if(event.text.unicode > 0 && event.text.unicode < 0x10000)
		{
			io.AddInputCharacter(static_cast<ImWchar>(event.text.unicode));
		}
	}
}

const mml::cursor* map_cursor(ImGuiMouseCursor cursor)
{
	static std::map<ImGuiMouseCursor, mml::cursor::type> cursor_map = {
		{ImGuiMouseCursor_None, mml::cursor::not_allowed},
		{ImGuiMouseCursor_Arrow, mml::cursor::arrow},
		{ImGuiMouseCursor_ResizeAll, mml::cursor::size_all},
		{ImGuiMouseCursor_TextInput, mml::cursor::text},
		{ImGuiMouseCursor_ResizeNS, mml::cursor::size_vertical},
		{ImGuiMouseCursor_ResizeEW, mml::cursor::size_horizontal},
		{ImGuiMouseCursor_ResizeNESW, mml::cursor::size_bottom_left_top_right},
		{ImGuiMouseCursor_ResizeNWSE, mml::cursor::size_top_left_bottom_right},

		{ImGuiMouseCursor_Hand, mml::cursor::hand},
		{ImGuiMouseCursor_NotAllowed, mml::cursor::not_allowed},
		{ImGuiMouseCursor_Help, mml::cursor::help},
		{ImGuiMouseCursor_Wait, mml::cursor::wait},
		{ImGuiMouseCursor_ArrowWait, mml::cursor::arrow_wait},
		{ImGuiMouseCursor_Cross, mml::cursor::cross},

	};
	auto id = cursor_map[cursor];
	static std::map<mml::cursor::type, std::unique_ptr<mml::cursor>> cursors;
	if(cursors.find(id) == cursors.end())
	{
		auto cursor = std::make_unique<mml::cursor>();
		if(cursor->load_from_system(id))
		{
			cursors.emplace(id, std::move(cursor));
		}
	}

	return cursors[id].get();
}

void imgui_frame_begin()
{
	gui::CleanupTextures();
}

void imgui_set_context(ImGuiContext* context)
{
	ImGuiContext* last_context = gui::GetCurrentContext();
	if(last_context != nullptr && last_context != context)
	{
		std::memcpy(&context->Style, &last_context->Style, sizeof(ImGuiStyle));
		std::memcpy(&context->IO.KeyMap, &last_context->IO.KeyMap, sizeof(last_context->IO.KeyMap));
		context->IO.IniFilename = last_context->IO.IniFilename;
		context->IO.ConfigFlags = last_context->IO.ConfigFlags;
		context->IO.FontAllowUserScaling = last_context->IO.FontAllowUserScaling;
		context->Initialized = last_context->Initialized;
	}
	gui::SetCurrentContext(context);
}

void imgui_frame_update(render_window& window, delta_t dt)
{
	auto& io = gui::GetIO();
	auto view_size = window.get_surface()->get_size();
	auto window_size = window.get_size();

	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2(static_cast<float>(view_size.width), static_cast<float>(view_size.height));
	// Setup time step
	io.DeltaTime = dt.count();

	irect32_t relative_rect;
	relative_rect.left = 0;
	relative_rect.top = 0;
	relative_rect.right = static_cast<std::int32_t>(window_size[0]);
	relative_rect.bottom = static_cast<std::int32_t>(window_size[1]);
	auto mouse_pos = mml::mouse::get_position(window);

	if(window.has_focus() && relative_rect.contains({mouse_pos[0], mouse_pos[1]}))
	{
		static auto last_cursor_type = gui::GetMouseCursor();
		auto cursor = map_cursor(gui::GetMouseCursor());
		if((cursor != nullptr) && last_cursor_type != gui::GetMouseCursor())
		{
			window.set_mouse_cursor(*cursor);
		}

		last_cursor_type = gui::GetMouseCursor();
	}

	// Start the frame
	gui::NewFrame();

	gui::SetNextWindowPos(ImVec2(0, 0));
	gui::SetNextWindowSize(io.DisplaySize);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
							 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
							 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

	gui::Begin("###workspace", nullptr, flags);
}

void imgui_frame_end()
{
	gui::End();
	gui::Render();
	render_func(gui::GetDrawData());
}

ImGuiContext* imgui_create_context(ImFontAtlas& atlas)
{
	return gui::CreateContext(&atlas);
}

void imgui_destroy_context(ImGuiContext* context = nullptr)
{
	gui::DestroyContext(context);
}

void imgui_init()
{
	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();

	auto vs_instance = std::make_shared<gfx::shader>(s_embedded_shaders, "vs_ocornut_imgui");
	auto fs_instance = std::make_shared<gfx::shader>(s_embedded_shaders, "fs_ocornut_imgui");

	auto vs_ocornut_imgui =
		am.load_asset_from_instance<gfx::shader>("embedded:/vs_ocornut_imgui", vs_instance);
	auto fs_ocornut_imgui =
		am.load_asset_from_instance<gfx::shader>("embedded:/fs_ocornut_imgui", fs_instance);

	ts.push_or_execute_on_owner_thread(
		[](asset_handle<gfx::shader> vs, asset_handle<gfx::shader> fs) {
			s_program = std::make_unique<gpu_program>(vs, fs);

		},
		vs_ocornut_imgui, fs_ocornut_imgui);

	ImGuiIO& io = gui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;
	// init keyboard mapping
	io.KeyMap[ImGuiKey_Tab] = mml::keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = mml::keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = mml::keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = mml::keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = mml::keyboard::Down;
	io.KeyMap[ImGuiKey_PageUp] = mml::keyboard::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = mml::keyboard::PageDown;
	io.KeyMap[ImGuiKey_Home] = mml::keyboard::Home;
	io.KeyMap[ImGuiKey_End] = mml::keyboard::End;
	io.KeyMap[ImGuiKey_Insert] = mml::keyboard::Insert;
	io.KeyMap[ImGuiKey_Delete] = mml::keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = mml::keyboard::BackSpace;
	io.KeyMap[ImGuiKey_Space] = mml::keyboard::Space;
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

	gui::AddFont("default", io.Fonts->AddFontDefault(&config));
	gui::AddFont("standard",
				 io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(std::intptr_t(s_font_default)),
												sizeof(s_font_default), 20, &config));

	static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
	config.MergeMode = true;
	config.PixelSnapH = true;
	gui::AddFont("icons", io.Fonts->AddFontFromMemoryTTF(
							  reinterpret_cast<void*>(std::intptr_t(fontawesome_webfont_ttf)),
							  sizeof(fontawesome_webfont_ttf), 20, &config, icons_ranges));

	config.MergeMode = false;
	config.PixelSnapH = false;
	gui::AddFont("standard_big",
				 io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(std::intptr_t(s_font_default)),
												sizeof(s_font_default), 50, &config));

	config.MergeMode = true;
	config.PixelSnapH = true;
	gui::AddFont("icons_big", io.Fonts->AddFontFromMemoryTTF(
								  reinterpret_cast<void*>(std::intptr_t(fontawesome_webfont_ttf)),
								  sizeof(fontawesome_webfont_ttf), 50, &config, icons_ranges));

	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	s_font_texture = std::make_shared<gfx::texture>(
		static_cast<std::uint16_t>(width), static_cast<std::uint16_t>(height), false, 1,
		gfx::texture_format::BGRA8, 0, gfx::copy(data, static_cast<std::uint32_t>(width * height * 4)));

	// Store our identifier
	io.Fonts->SetTexID(s_font_texture.get());
	get_gui_style().load_style();
}

void imgui_dispose()
{
	gui::ClearFonts();
	gui::CleanupTextures();
	s_program.reset();
	s_font_texture.reset();
}
}

gui_system::gui_system()
{
	runtime::on_platform_events.connect(this, &gui_system::platform_events);
	runtime::on_frame_begin.connect(this, &gui_system::frame_begin);

	initial_context_ = imgui_create_context(atlas_);
	imgui_init();
}

gui_system::~gui_system()
{
	runtime::on_platform_events.connect(this, &gui_system::platform_events);
	runtime::on_frame_begin.disconnect(this, &gui_system::frame_begin);

	imgui_dispose();
	imgui_destroy_context(initial_context_);
}

void gui_system::frame_begin(delta_t /*unused*/)
{
	imgui_frame_begin();
}

uint32_t gui_system::get_draw_calls() const
{
	return s_draw_calls;
}

ImGuiContext* gui_system::get_context(uint32_t id)
{
	auto it = contexts_.find(id);
	if(it != contexts_.end())
	{
		return it->second;
	}

	auto ins = contexts_.emplace(id, imgui_create_context(atlas_));
	return ins.first->second;
}

void gui_system::push_context(uint32_t id)
{
	auto context = get_context(id);

	imgui_set_context(context);
}

void gui_system::draw_begin(render_window& window, delta_t dt)
{
	imgui_frame_update(window, dt);
}

void gui_system::draw_end()
{
	imgui_frame_end();
}

void gui_system::pop_context()
{
	imgui_set_context(initial_context_);
}

void gui_system::platform_events(const std::pair<std::uint32_t, bool>& info,
								 const std::vector<mml::platform_event>& events)
{
	const auto window_id = info.first;
	push_context(window_id);
	for(const auto& e : events)
	{
		if(e.type == mml::platform_event::closed)
		{
			pop_context();
			auto context = contexts_[window_id];
			if(context != nullptr)
			{
				imgui_destroy_context(context);
			}
			contexts_.erase(window_id);
			return;
		}

		imgui_handle_event(e);
	}
	pop_context();
}

gui_style& get_gui_style()
{
	static gui_style s_gui_style;
	return s_gui_style;
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
	style.Colors[ImGuiCol_ChildBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x * 0.8f, col_area.y * 0.8f, col_area.z * 0.8f, 1.00f);
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
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.10f, 0.10f, 0.10f, 0.55f);
}

void gui_style::load_style()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor:/config/style.cfg");
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
	const fs::path absoluteKey = fs::resolve_protocol("editor:/config/style.cfg");
	std::ofstream output(absoluteKey.string());
	cereal::oarchive_associative_t ar(output);

	try_save(ar, cereal::make_nvp("style", setup));
}

gui_style::hsv_setup gui_style::get_dark_style()
{
	hsv_setup result;
	result.col_main_hue = 0.0f / 255.0f;
	result.col_main_sat = 0.0f / 255.0f;
	result.col_main_val = 80.0f / 255.0f;

	result.col_area_hue = 0.0f / 255.0f;
	result.col_area_sat = 0.0f / 255.0f;
	result.col_area_val = 50.0f / 255.0f;

	result.col_back_hue = 0.0f / 255.0f;
	result.col_back_sat = 0.0f / 255.0f;
	result.col_back_val = 35.0f / 255.0f;

	result.col_text_hue = 0.0f / 255.0f;
	result.col_text_sat = 0.0f / 255.0f;
	result.col_text_val = 255.0f / 255.0f;
	result.frame_rounding = 0.0f;

	return result;
}

gui_style::hsv_setup gui_style::get_lighter_red()
{
	hsv_setup result;
	result.col_main_hue = 0.0f / 255.0f;
	result.col_main_sat = 200.0f / 255.0f;
	result.col_main_val = 170.0f / 255.0f;

	result.col_area_hue = 0.0f / 255.0f;
	result.col_area_sat = 0.0f / 255.0f;
	result.col_area_val = 80.0f / 255.0f;

	result.col_back_hue = 0.0f / 255.0f;
	result.col_back_sat = 0.0f / 255.0f;
	result.col_back_val = 35.0f / 255.0f;

	result.col_text_hue = 0.0f / 255.0f;
	result.col_text_sat = 0.0f / 255.0f;
	result.col_text_val = 255.0f / 255.0f;
	result.frame_rounding = 0.0f;

	return result;
}
