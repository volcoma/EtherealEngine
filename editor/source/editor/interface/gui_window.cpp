#include "gui_window.h"
#include "runtime/rendering/render_pass.h"
#include "../editing/editing_system.h"

void handle_sfml_event(sf::Event event)
{
	auto& io = gui::GetIO();
	if (event.type == sf::Event::LostFocus)
	{
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		io.MouseDown[2] = false;
	}

	if (event.type == sf::Event::KeyPressed)
	{
		io.KeysDown[event.key.code] = true;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if (event.type == sf::Event::KeyReleased)
	{
		io.KeysDown[event.key.code] = false;
		io.KeyAlt = event.key.alt;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeySuper = event.key.system;
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		io.MouseWheel += event.mouseWheelScroll.delta;
	}

	if (event.type == sf::Event::MouseButtonPressed)
	{
		io.MouseDown[event.mouseButton.button] = true;
	}

	if (event.type == sf::Event::MouseButtonReleased)
	{
		io.MouseDown[event.mouseButton.button] = false;
	}

	if (event.type == sf::Event::MouseMoved)
	{
		io.MousePos.x = float(event.mouseMove.x);
		io.MousePos.y = float(event.mouseMove.y);
	}

	if (event.type == sf::Event::TextEntered)
	{
		if (event.text.unicode > 0 && event.text.unicode < 0x10000)
			io.AddInputCharacter(event.text.unicode);
	}
}

sf::Window::Cursor map_cursor(ImGuiMouseCursor cursor)
{
	static std::map<ImGuiMouseCursor_, sf::Window::Cursor> cursor_map = {
		{ ImGuiMouseCursor_Arrow, sf::Window::Arrow },
		{ ImGuiMouseCursor_Move, sf::Window::Hand },
		{ ImGuiMouseCursor_NotAllowed, sf::Window::NotAllowed },
		{ ImGuiMouseCursor_Help, sf::Window::Help },
		{ ImGuiMouseCursor_TextInput, sf::Window::Text },
		{ ImGuiMouseCursor_ResizeNS, sf::Window::SizeVertical },
		{ ImGuiMouseCursor_ResizeEW, sf::Window::SizeHorizontal },
		{ ImGuiMouseCursor_ResizeNESW, sf::Window::SizeBottomLeftTopRight },
		{ ImGuiMouseCursor_ResizeNWSE, sf::Window::SizeTopLeftBottomRight }
	};

	return cursor_map[static_cast<ImGuiMouseCursor_>(cursor)];
}

void imgui_frame_update(render_window& window, std::chrono::duration<float> dt, const usize& view_size)
{
	auto& io = gui::GetIO();
	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2(static_cast<float>(view_size.width), static_cast<float>(view_size.height));
	// Setup time step
	io.DeltaTime = dt.count();

	auto window_pos = window.getPosition();
	auto window_size = window.getSize();
	irect rect;
	rect.left = window_pos.x;
	rect.top = window_pos.y;
	rect.right = window_size.width;
	rect.bottom = window_size.height;
	auto mouse_pos = sf::Mouse::getPosition(window);

	if (window.hasFocus() && rect.contains(mouse_pos))
		window.setMouseCursor(map_cursor(gui::GetMouseCursor()));

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

gui_window::gui_window(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
: render_window(mode, title, style)
, _dockspace(this)
{
	_gui_context = imgui_create_context();
}

gui_window::~gui_window()
{
	imgui_destroy_context(_gui_context);
}

bool gui_window::filterEvent(const sf::Event& event)
{
	handle_sfml_event(event);

	return render_window::filterEvent(event);;
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
	_dockspace.update_and_draw(gui::GetContentRegionAvail());
}

void gui_window::frame_end()
{
	render_window::frame_end();

	auto es = core::get_subsystem<editor::editing_system>();

	if (gui::IsMouseDragging(gui::drag_button) && es->drag_data.object)
	{
		gui::SetTooltip(es->drag_data.description.c_str());

		if (gui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
			gui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
	}

	if (!gui::IsAnyItemActive() && !gui::IsAnyItemHovered())
	{
		if (gui::IsMouseDoubleClicked(0) && !imguizmo::is_over())
		{
			es->unselect();
			es->drop();
		}
	}
	if (gui::IsMouseReleased(gui::drag_button))
	{
		es->drop();
	}

	imgui_frame_end();
}