#include "GuiWindow.h"
#include "Runtime/Rendering/RenderPass.h"

void handleSFMLEvent(sf::Event event)
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

void imguiFrameUpdate(RenderWindow& window, float dt, const uSize& viewSize)
{
	auto& io = gui::GetIO();
	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2(static_cast<float>(viewSize.width), static_cast<float>(viewSize.height));
	// Setup time step
	io.DeltaTime = dt;

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

void imguiFrameEnd()
{
	gui::End();
	gui::Render();
}

ImGuiContext* imguiCreateContext()
{
	return gui::CreateContext();
}

void imguiDestroyContext(ImGuiContext* pContext)
{
	if (pContext)
		gui::DestroyContext(pContext);
}

void imguiFrameBegin(ImGuiContext* pContext)
{
	ImGuiContext* prevContext = gui::GetCurrentContext();
	if (prevContext)
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
: mDockspace(this)
{
	mGuiContext = imguiCreateContext();
}

GuiWindow::GuiWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
: RenderWindow(mode, title, style)
, mDockspace(this)
{
	mGuiContext = imguiCreateContext();
}

GuiWindow::~GuiWindow()
{
	imguiDestroyContext(mGuiContext);
}

bool GuiWindow::filterEvent(const sf::Event& event)
{
	handleSFMLEvent(event);

	return RenderWindow::filterEvent(event);;
}

void GuiWindow::frameBegin()
{
	RenderWindow::frameBegin();

	imguiFrameBegin(mGuiContext);
}

void GuiWindow::frameUpdate(float dt)
{
	RenderWindow::frameUpdate(dt);

	const auto size = mSurface->getSize();
	
	imguiFrameUpdate(*this, dt, size);
}

void GuiWindow::frameRender()
{
	RenderWindow::frameRender();
	mDockspace.updateAndDraw(gui::GetContentRegionAvail());
}

void GuiWindow::frameEnd()
{
	RenderWindow::frameEnd();
	imguiFrameEnd();
}