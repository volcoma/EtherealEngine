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
		Dock* activeDock = nullptr;
		//Nodes can have multiple tabbed docks
		std::vector<Dock*> docks;

		// What kind of split is it
		bool verticalSplit = false;

		bool alwaysAutoResize = true;

		//size of the node in pixels along its split
		float size = 0;
	};

	struct Dock
	{
		Dock* initialize(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize, std::function<void(ImVec2)> ddrawFunction)
		{
			title = dtitle;
			closeButton = dcloseButton;
			minSize = dminSize;
			drawFunction = ddrawFunction;
			return this;
		};
		~Dock()
		{
			title.clear();
		}

		//Container *parent = nullptr;
		Node* container = nullptr;
		Dockspace* redockFrom = nullptr;
		GuiWindow* redockFromWindow = nullptr;
		Dock* redockTo = nullptr;

		DockSlot redockSlot = DockSlot::None;
		bool closeButton = true;
		bool undockable = false;
		bool draging = false;

		ImVec2 lastSize;
		ImVec2 minSize;

		std::string title;
		std::function<void(ImVec2)> drawFunction;
		std::function<bool(void)> onCloseFunction;
	};

	class Dockspace
	{
	public:
		Dockspace(GuiWindow* owner);
		~Dockspace();

		bool dock(Dock* dock, DockSlot dockSlot, float size = 0, bool active = false);
		bool dockWith(Dock* dock, Dock* dockTo, DockSlot dockSlot, float size = 0, bool active = false);
		bool undock(Dock* dock);

		void updateAndDraw(ImVec2 size);
		void clear();
		bool hasDock(const std::string& name);
		Node node;
		std::vector<Node*> nodes;
	protected:
		friend class ::GuiWindow;

		DockSlot _renderDockSlotPreview(const ImVec2& mousePos, const ImVec2& cPos, const ImVec2& cSize);
		void _renderTabBar(Node* container, const ImVec2& size, const ImVec2& cursorPos);
		bool _getMinSize(Node* container, ImVec2& min);
		GuiWindow* _isAnyWindowDragging();

		enum DockToAction
		{
			eUndock, eDrag, eClose, eNull
		};

		Dock* mCurrentDockTo = nullptr;
		DockToAction mCurrentDockAction = eNull;
	public:
		GuiWindow* mOwner = nullptr;
	};
};