#include "gui_window.h"
#include "runtime/rendering/render_pass.h"

void handle_sfml_event(mml::platform_event event)
{
	auto& io = gui::GetIO();
	if (event.type == mml::platform_event::lost_focus)
	{
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		io.MouseDown[2] = false;
	}

	if (event.type == mml::platform_event::key_pressed)
	{
		io.KeysDown[event.key.code] = true;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if (event.type == mml::platform_event::key_released)
	{
		io.KeysDown[event.key.code] = false;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if (event.type == mml::platform_event::mouse_wheel_scrolled)
	{
		io.MouseWheel += event.mouse_wheel_scroll.delta;
	}

	if (event.type == mml::platform_event::mouse_button_pressed)
	{
		io.MouseDown[event.mouse_button.button] = true;
	}

	if (event.type == mml::platform_event::mouse_button_released)
	{
		io.MouseDown[event.mouse_button.button] = false;
	}

	if (event.type == mml::platform_event::mouse_moved)
	{
		io.MousePos.x = float(event.mouse_move.x);
		io.MousePos.y = float(event.mouse_move.y);
	}

	if (event.type == mml::platform_event::text_entered)
	{
		if (event.text.unicode > 0 && event.text.unicode < 0x10000)
			io.AddInputCharacter(event.text.unicode);
	}
}

const mml::cursor* map_cursor(ImGuiMouseCursor cursor)
{
	static std::map<ImGuiMouseCursor_, mml::cursor::type> cursor_map = {
		{ ImGuiMouseCursor_Arrow, mml::cursor::arrow },
		{ ImGuiMouseCursor_Move, mml::cursor::hand },
		{ ImGuiMouseCursor_NotAllowed, mml::cursor::not_allowed },
		{ ImGuiMouseCursor_Help, mml::cursor::help },
		{ ImGuiMouseCursor_TextInput, mml::cursor::text },
		{ ImGuiMouseCursor_ResizeNS, mml::cursor::size_vertical },
		{ ImGuiMouseCursor_ResizeEW, mml::cursor::size_horizontal },
		{ ImGuiMouseCursor_ResizeNESW, mml::cursor::size_bottom_left_top_right },
		{ ImGuiMouseCursor_ResizeNWSE, mml::cursor::size_top_left_bottom_right }
	};
	auto id = cursor_map[static_cast<ImGuiMouseCursor_>(cursor)];
	static std::map<mml::cursor::type, std::unique_ptr<mml::cursor>> cursors;
	if (cursors.find(id) == cursors.end())
	{
		auto cursor = std::make_unique<mml::cursor>();
		if (cursor->load_from_system(id))
		{
			cursors.emplace(id, std::move(cursor));
		}
	}

	return cursors[id].get();
}

void imgui_frame_update(render_window& window, std::chrono::duration<float> dt, const usize& view_size)
{
	auto& io = gui::GetIO();
	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2(static_cast<float>(view_size.width), static_cast<float>(view_size.height));
	// Setup time step
	io.DeltaTime = dt.count();

	auto window_pos = window.get_position();
	auto window_size = window.get_size();
	irect rect;
	rect.left = window_pos[0];
	rect.top = window_pos[1];
	rect.right = window_size[0];
	rect.bottom = window_size[1];
	auto mouse_pos = mml::mouse::get_position(window);

	if (window.has_focus() && rect.contains({ mouse_pos[0], mouse_pos[1] }))
	{
		auto cursor = map_cursor(gui::GetMouseCursor());
		if(cursor)
			window.set_mouse_cursor(*cursor);
	}
		

	// Start the frame	
	gui::NewFrame();

	gui::SetNextWindowPos(ImVec2(0, 0));
	gui::SetNextWindowSize(ImVec2(static_cast<float>(view_size.width), static_cast<float>(view_size.height)));
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoFocusOnAppearing
		;

	gui::Begin("###workspace", 0, flags);
}

void imgui_frame_end()
{
	gui::End();
	gui::Render();
}

ImGuiContext* imgui_create_context()
{
	return gui::CreateContext();
}

void imgui_destroy_context(ImGuiContext* pContext)
{
	if (pContext)
		gui::DestroyContext(pContext);
}

void imgui_set_context(ImGuiContext* pContext)
{
	ImGuiContext* prevContext = gui::GetCurrentContext();
	if (prevContext != nullptr && prevContext != pContext)
	{
		std::memcpy(&pContext->Style, &prevContext->Style, sizeof(ImGuiStyle));
		std::memcpy(&pContext->IO.KeyMap, &prevContext->IO.KeyMap, sizeof(prevContext->IO.KeyMap));
		std::memcpy(&pContext->MouseCursorData, &prevContext->MouseCursorData, sizeof(pContext->MouseCursorData));
		pContext->IO.IniFilename = prevContext->IO.IniFilename;
		pContext->IO.FontAllowUserScaling = prevContext->IO.FontAllowUserScaling;
		pContext->IO.RenderDrawListsFn = prevContext->IO.RenderDrawListsFn;
		pContext->Initialized = prevContext->Initialized;
	}
	gui::SetCurrentContext(pContext);
}


gui_window::gui_window() 
: _dockspace(this)
{
	_gui_context = imgui_create_context();
}

gui_window::gui_window(mml::video_mode mode, const std::string& title, std::uint32_t style /*= mml::style::Default*/)
: render_window(mode, title, style)
, _dockspace(this)
{
	_gui_context = imgui_create_context();
}

gui_window::~gui_window()
{
	imgui_destroy_context(_gui_context);
}

bool gui_window::filter_event(const mml::platform_event& event)
{
	imgui_set_context(_gui_context);

	handle_sfml_event(event);

	return render_window::filter_event(event);;
}

void gui_window::frame_begin()
{
	render_window::frame_begin();

	imgui_set_context(_gui_context);
}

void gui_window::frame_update(std::chrono::duration<float> dt)
{
	render_window::frame_update(dt);

	imgui_frame_update(*this, dt, _surface->get_size());
}

void gui_window::frame_render(std::chrono::duration<float> dt)
{
	render_window::frame_render(dt);

	imgui_set_context(_gui_context);

	on_gui(dt);

	render_dockspace();
}

void gui_window::render_dockspace()
{
	_dockspace.update_and_draw(ImVec2(gui::GetContentRegionAvail()));	
}

void gui_window::frame_end()
{
	render_window::frame_end();

	imgui_frame_end();
}