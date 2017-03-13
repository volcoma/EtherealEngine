#pragma once
#include <vector>
#include <functional>
#include <string>
#include "../gui_system.h"
class GuiWindow;

namespace ImGuiDock
{
	enum class DockSlot { Left, Right, Top, Bottom, Tab, None };

	struct Dock;
	class Dockspace;

	struct Node
	{
		Node* splits[2] = { nullptr, nullptr };
		Node* parent = nullptr;

		// Only one dock is active at a time
		Dock* active_dock = nullptr;
		//Nodes can have multiple tabbed docks
		std::vector<Dock*> docks;

		// What kind of split is it
		bool vertical_split = false;

		bool always_auto_resize = true;

		//size of the node in pixels along its split
		float size = 0;
	};

	struct Dock
	{
		void initialize(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize, std::function<void(ImVec2)> ddrawFunction)
		{
			title = dtitle;
			close_button = dcloseButton;
			min_size = dminSize;
			draw_function = ddrawFunction;
		};

		virtual ~Dock() {}

		//Container *parent = nullptr;
		Node* container = nullptr;
		Dockspace* redock_from = nullptr;
		GuiWindow* redock_from_window = nullptr;
		Dock* redock_to = nullptr;

		DockSlot redock_slot = DockSlot::None;
		bool close_button = true;
		bool undockable = false;
		bool draging = false;

		ImVec2 last_size;
		ImVec2 min_size;

		std::string title;
		std::function<void(ImVec2)> draw_function;
		std::function<bool(void)> on_close_func;
	};

	class Dockspace
	{
	public:
		Dockspace(GuiWindow* owner);
		~Dockspace();

		bool dock(Dock* dock, DockSlot dockSlot, float size = 0, bool active = false);
		bool dock_with(Dock* dock, Dock* dockTo, DockSlot dockSlot, float size = 0, bool active = false);
		bool undock(Dock* dock);

		void update_and_draw(ImVec2 size);
		void clear();
		bool has_dock(const std::string& name);
		Node node;
		std::vector<Node*> nodes;
	protected:
		friend class ::GuiWindow;

		DockSlot render_dock_slot_preview(const ImVec2& mousePos, const ImVec2& cPos, const ImVec2& cSize);
		void render_container(uint32_t& idgen, Node* container, ImVec2 size, ImVec2 cursor_pos);
		void render_tab_bar(Node* container, const ImVec2& size, const ImVec2& cursorPos);
		bool get_min_size(Node* container, ImVec2& min);
		GuiWindow* is_any_window_dragged();

		enum DockToAction
		{
			eUndock, eDrag, eClose, eNull
		};

		Dock* _current_dock_to = nullptr;
		DockToAction _current_dock_action = eNull;
	public:
		GuiWindow* owner = nullptr;
	};
};