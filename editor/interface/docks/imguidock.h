#pragma once
#include <vector>
#include <functional>
#include <string>
#include "../gui_system.h"
class gui_window;

namespace imguidock
{
	enum class slot 
	{ 
		left,
		right,
		top,
		bottom,
		tab,
		none 
	};

	struct dock;
	class dockspace;

	struct node
	{
		node* splits[2] = { nullptr, nullptr };
		node* parent = nullptr;

		// Only one dock is active at a time
		dock* active_dock = nullptr;
		//Nodes can have multiple tabbed docks
		std::vector<dock*> docks;

		// What kind of split is it
		bool vertical_split = false;

		bool always_auto_resize = true;

		//size of the node in pixels along its split
		float size = 0;
	};

	struct dock
	{
		void initialize(
			const std::string& dtitle, 
			bool dcloseButton, 
			const ImVec2& dminSize, 
			std::function<void(const ImVec2&)> ddrawFunction);

		virtual ~dock() = default;

		//Container *parent = nullptr;
		node* container = nullptr;
		dockspace* redock_from = nullptr;
		gui_window* redock_from_window = nullptr;
		dock* redock_to = nullptr;

		slot redock_slot = slot::none;
		bool close_button = true;
		bool undockable = false;
		bool draging = false;

		ImVec2 last_size;
		ImVec2 min_size;

		std::string title;
		std::function<void(const ImVec2&)> draw_function;
		std::function<bool(void)> on_close_func;
	};

	class dockspace
	{
	public:
		dockspace(gui_window* owner);
		~dockspace();

		bool dock_to(dock* ddock, slot dockSlot, float size = 0, bool active = false);
		bool dock_with(dock* ddock, dock* dockTo, slot dockSlot, float size = 0, bool active = false);
		bool undock(dock* ddock);

		void update_and_draw(ImVec2 size);
		void clear();
		void activate_dock(const std::string& name);

		node root;
		std::vector<node*> nodes;
	protected:
		friend class ::gui_window;

		slot render_dock_slot_preview(const ImVec2& mousePos, const ImVec2& cPos, const ImVec2& cSize);
		void render_container(uint32_t& idgen, node* container, ImVec2 size, ImVec2 cursor_pos);
		void render_tab_bar(node* container, const ImVec2& size, const ImVec2& cursorPos, float tabbar_height);
		bool get_min_size(node* container, ImVec2& min);
		gui_window* is_any_window_dragged();

		enum DockToAction
		{
			eUndock, eDrag, eClose, eNull
		};

		dock* _current_dock_to = nullptr;
		DockToAction _current_dock_action = eNull;
	public:
		gui_window* owner = nullptr;
	};
};