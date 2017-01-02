#include "gui_window.h"
#include "runtime/rendering/render_pass.h"

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

void imgui_frame_update(RenderWindow& window, std::chrono::duration<float> dt, const uSize& viewSize)
{
	auto& io = gui::GetIO();
	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2(static_cast<float>(viewSize.width), static_cast<float>(viewSize.height));
	// Setup time step
	io.DeltaTime = dt.count();

	if (window.hasFocus())
	{
		switch (gui::GetMouseCursor())
		{
		case ImGuiMouseCursor_Arrow:
			window.setMouseCursor(sf::Window::Arrow);
			break;
		case ImGuiMouseCursor_Move:
			window.setMouseCursor(sf::Window::Hand);
			break;
		case ImGuiMouseCursor_NotAllowed:
			window.setMouseCursor(sf::Window::NotAllowed);
			break;
		case ImGuiMouseCursor_Help:
			window.setMouseCursor(sf::Window::Help);
			break;
		case ImGuiMouseCursor_TextInput:			// When hovering over InputText, etc.
			window.setMouseCursor(sf::Window::Text);
			break;
		case ImGuiMouseCursor_ResizeNS:				// Unused
			window.setMouseCursor(sf::Window::SizeVertical);
			break;
		case ImGuiMouseCursor_ResizeEW:				// When hovering over a column
			window.setMouseCursor(sf::Window::SizeHorizontal);
			break;
		case ImGuiMouseCursor_ResizeNESW:			// Unused
			window.setMouseCursor(sf::Window::SizeBottomLeftTopRight);
			break;
		case ImGuiMouseCursor_ResizeNWSE:			// When hovering over the bottom-right corner of a window
			window.setMouseCursor(sf::Window::SizeTopLeftBottomRight);
			break;
		default: break;
		}
	}
	

	// Start the frame	
	gui::NewFrame();

	gui::SetNextWindowPos(ImVec2(0, 0));
	gui::SetNextWindowSize(ImVec2(static_cast<float>(viewSize.width), static_cast<float>(viewSize.height)));
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
		pContext->IO.RenderDrawListsFn = prevContext->IO.RenderDrawListsFn;
		pContext->Initialized = prevContext->Initialized;
	}
	gui::SetCurrentContext(pContext);
}


GuiWindow::GuiWindow() 
: _dockspace(this)
{
	_gui_context = imgui_create_context();
}

GuiWindow::GuiWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
: RenderWindow(mode, title, style)
, _dockspace(this)
{
	_gui_context = imgui_create_context();
}

GuiWindow::~GuiWindow()
{
	imgui_destroy_context(_gui_context);
}

bool GuiWindow::filterEvent(const sf::Event& event)
{
	handle_sfml_event(event);

	return RenderWindow::filterEvent(event);;
}

void GuiWindow::frame_begin()
{
	RenderWindow::frame_begin();

	imgui_set_context(_gui_context);
}

void GuiWindow::frame_update(std::chrono::duration<float> dt)
{
	RenderWindow::frame_update(dt);

	imgui_frame_update(*this, dt, _surface->get_size());
}

void GuiWindow::frame_render(std::chrono::duration<float> dt)
{
	RenderWindow::frame_render(dt);

	imgui_set_context(_gui_context);

	on_gui(dt);

	_dockspace.update_and_draw(gui::GetContentRegionAvail());
}

#include "../edit_state.h"

void GuiWindow::frame_end()
{
	RenderWindow::frame_end();

	auto es = core::get_subsystem<editor::EditState>();

	if (gui::IsMouseDragging(gui::drag_button) && es->drag_data.object)
	{
		gui::SetTooltip(es->drag_data.description.c_str());
	}
	
	if (!gui::IsAnyItemHovered())
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