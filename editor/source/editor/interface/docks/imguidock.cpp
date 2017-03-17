#define IMGUI_DEFINE_MATH_OPERATORS
#include "imguidock.h"
#include "../gui_window.h"
#include "runtime/system/engine.h"

namespace ImGuiDock
{
	Dockspace::Dockspace(GuiWindow* owner) : owner(owner)
	{

	}

	Dockspace::~Dockspace()
	{
		clear();
	}

	bool Dockspace::dock(Dock* dock, DockSlot dock_slot, float size, bool active)
	{
		return dock_with(dock, nullptr, dock_slot, size, active);
	}

	bool Dockspace::dock_with(Dock* dock, Dock* dock_to, DockSlot dock_slot, float size, bool active)
	{
		if (dock == nullptr)
			return false;

		Node* current_container = &node;

		if (dock_to != nullptr)
		{
			if (dock_slot == ImGuiDock::DockSlot::Tab)
			{
				dock_to->container->active_dock = active ? dock : current_container->splits[0]->active_dock ? current_container->splits[0]->active_dock : dock;
				dock_to->container->docks.push_back(dock);
				dock->container = dock_to->container;
				return true;
			}
			else
			{
				nodes.push_back(new Node{});
				auto new_container = nodes[nodes.size() - 1];
				new_container->parent = dock_to->container->parent;
				new_container->splits[0] = dock_to->container;
				new_container->size = dock_to->container->size;
				//if (size)
				//	new_container->alwaysAutoResize = false;
				dock_to->container->size = 0;
				if (dock_to->container->parent->splits[0] == dock_to->container)
					dock_to->container->parent->splits[0] = new_container;
				else dock_to->container->parent->splits[1] = new_container;
				//dock_to->container->parent = new_container;
				dock_to->container = new_container->splits[0];
				dock_to->container->parent = new_container;
				current_container = new_container;
			}
		}

		Node *child_container = nullptr;
		if (current_container->splits[0] == nullptr || current_container->splits[1] == nullptr)
		{
			nodes.push_back(new Node{});
			child_container = nodes[nodes.size() - 1];
		};

		if (current_container->splits[0] == nullptr)
		{
			current_container->splits[0] = child_container;
			current_container->splits[0]->active_dock = active ? dock : current_container->splits[0]->active_dock ? current_container->splits[0]->active_dock : dock;
			current_container->splits[0]->docks.push_back(dock);
			current_container->splits[0]->parent = current_container;
			current_container->splits[0]->size = size < 0 ? size * -1 : size;
			dock->container = current_container->splits[0];
			dock->container->parent = current_container;
		}
		else if (current_container->splits[1] == nullptr)
		{
			current_container->splits[1] = child_container;
			//Node *otherSplit = current_container->splits[0];
			if (size > 0)
			{
				current_container->splits[0]->always_auto_resize = true;
				current_container->splits[0]->size = 0;
				current_container->splits[1]->size = size;
				current_container->splits[1]->always_auto_resize = false;
			}
			else if (size == 0) {}
			else
			{
				current_container->splits[0]->always_auto_resize = false;
				current_container->splits[0]->size = size * -1;
				current_container->splits[1]->size = 0;
				current_container->splits[1]->always_auto_resize = true;
			}
			switch (dock_slot)
			{
			case ImGuiDock::DockSlot::Left:
				current_container->splits[1] = current_container->splits[0];
				current_container->splits[0] = child_container;
				current_container->vertical_split = true;
				break;
			case ImGuiDock::DockSlot::Right:
				current_container->vertical_split = true;
				break;
			case ImGuiDock::DockSlot::Top:
				current_container->splits[1] = current_container->splits[0];
				current_container->splits[0] = child_container;
				current_container->vertical_split = false;
				break;
			case ImGuiDock::DockSlot::Bottom:
				current_container->vertical_split = false;
				break;
			case ImGuiDock::DockSlot::Tab:
				current_container->vertical_split = false;
				break;
			case ImGuiDock::DockSlot::None:
				break;
			default:
				break;
			}
			child_container->active_dock = active ? dock : child_container->active_dock ? child_container->active_dock : dock;
			child_container->docks.push_back(dock);
			child_container->parent = current_container;

			//	if (child_container->parent != nullptr && current_container->verticalSplit != child_container->parent->verticalSplit)
			//		current_container->size = otherSplit->size ? otherSplit->size + otherSplit->size : otherSplit->size;

			dock->container = child_container;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool Dockspace::undock(Dock *dock)
	{
		if (dock != nullptr)
		{
			if (dock->container->docks.size() > 1)
			{
				for (size_t i = 0; i < dock->container->docks.size(); ++i)
				{
					if (dock->container->docks[i] == dock)
					{
						dock->last_size = dock->container->active_dock->last_size;
						dock->container->docks.erase(dock->container->docks.begin() + i);
						if (i != dock->container->docks.size())
							dock->container->active_dock = dock->container->docks[i];
						else dock->container->active_dock = dock->container->docks[i - 1];
					}
				}
			}
			else
			{
				Node *to_delete = nullptr, *parent_to_delete = nullptr;
				if (dock->container->parent == &node)
				{
					if (node.splits[0] == dock->container)
					{
						if (node.splits[1])
						{
							to_delete = node.splits[0];
							if (node.splits[1]->splits[0]) {
								parent_to_delete = node.splits[1];
								node.splits[0] = node.splits[1]->splits[0];
								node.splits[0]->parent = &node;
								node.splits[0]->vertical_split = false;
								node.splits[1] = node.splits[1]->splits[1];
								node.splits[1]->parent = &node;
								node.splits[1]->parent->vertical_split = node.splits[1]->vertical_split;
								node.splits[1]->vertical_split = false;
							}
							else
							{
								node.splits[0] = node.splits[1];
								node.splits[1] = nullptr;
								node.splits[0]->size = 0;
								node.splits[0]->vertical_split = false;
								node.splits[0]->parent->vertical_split = false;
							}
						}
						else return false;
					}
					else
					{
						to_delete = node.splits[1];
						node.splits[1] = nullptr;
					}
				}
				else
				{
					parent_to_delete = dock->container->parent;
					if (dock->container->parent->splits[0] == dock->container)
					{
						to_delete = dock->container->parent->splits[0];
						Node *parent = dock->container->parent->parent;
						Node *working = nullptr;
						if (dock->container->parent->parent->splits[0] == dock->container->parent)
							working = dock->container->parent->parent->splits[0] = dock->container->parent->splits[1];
						else working = dock->container->parent->parent->splits[1] = dock->container->parent->splits[1];
						working->parent = parent;
						working->size = dock->container->parent->size;
					}
					else
					{
						to_delete = dock->container->parent->splits[1];
						Node *parent = dock->container->parent->parent;
						Node *working = nullptr;
						if (dock->container->parent->parent->splits[0] == dock->container->parent)
							working = dock->container->parent->parent->splits[0] = dock->container->parent->splits[0];
						else working = dock->container->parent->parent->splits[1] = dock->container->parent->splits[0];
						working->parent = parent;
						working->size = dock->container->parent->size;
					}
				}
				for (size_t i = 0; i < nodes.size(); ++i)
				{
					if (to_delete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
						break;
					}
					if (nodes.size() > 1 && parent_to_delete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
						break;
					}
					if (nodes.size() > 1 && to_delete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
						break;
					}
				}
			}
			return true;
		}
		return false;
	}

	void updateDrag(GuiWindow* window)
	{
		auto& dockspace = window->get_dockspace();
		auto& split = dockspace.node.splits[0];

		if (split && split->active_dock)
		{
			auto& active_dock = split->active_dock;
			bool mouseleft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
			if (active_dock->draging && mouseleft)
			{
				auto pos = sf::Mouse::getPosition();
				pos.x -= 20;
				pos.y -= 40;
				window->setPosition(pos);
				window->setAlpha(0.3f);
			}
			else
			{
				if (active_dock->draging)
				{
					if (active_dock->redock_to)
					{
						active_dock->container = nullptr;
						active_dock->redock_from->dock_with(
							active_dock,
							active_dock->redock_to,
							active_dock->redock_slot, 0, true);
						active_dock->redock_to = nullptr;

						window->close();
					}
					else
					{
						window->setAlpha(1.0f);
					}
				}
				active_dock->draging = false;
			}
		}
	}

	void Dockspace::render_container(uint32_t& idgen, Node* container, ImVec2 size, ImVec2 cursor_pos)
	{	
		const float tabbar_height = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 calculated_size = size;
		ImVec2 calculated_cursor_pos = cursor_pos;

		idgen++;

		std::string idname = "Dock##";
		idname += idgen;

		calculated_size.y -= tabbar_height;

		float splitter_button_width = 4;
		float splitterButtonWidthHalf = splitter_button_width / 2;

		if (container->splits[0] == nullptr && container != &node)
		{
			render_tab_bar(container, calculated_size, cursor_pos, tabbar_height);
			cursor_pos.y += tabbar_height;

			ImGui::SetCursorPos(cursor_pos);
			ImVec2 screen_cursor_pos = ImGui::GetCursorScreenPos();
			screen_cursor_pos.y -= tabbar_height;

			ImGui::BeginChild(idname.c_str(), calculated_size, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			container->active_dock->draw_function(calculated_size);
			container->active_dock->last_size = calculated_size;

			ImGui::EndChild();

			GuiWindow* dragged_window = is_any_window_dragged();
			if (dragged_window != nullptr && dragged_window->get_dockspace().node.splits[0]->active_dock != container->active_dock)
			{
				auto mouse_pos = sf::Mouse::getPosition(*owner);
				ImVec2 cursor_pos = { float(mouse_pos.x), float(mouse_pos.y) };
				if ((mouse_pos.x > screen_cursor_pos.x && mouse_pos.x < (screen_cursor_pos.x + size.x)) &&
					(mouse_pos.y > screen_cursor_pos.y && mouse_pos.y < (screen_cursor_pos.y + size.y)))
				{
					ImGui::BeginChild("##dockSlotPreview");
					ImGui::PushClipRect(ImVec2(), ImGui::GetIO().DisplaySize, false);
					DockSlot dock_slot = render_dock_slot_preview(cursor_pos, screen_cursor_pos, size);
					ImGui::PopClipRect();
					ImGui::EndChild();
					if (dock_slot != DockSlot::None)
					{
						Dock *dragged_window_dock = dragged_window->get_dockspace().node.splits[0]->active_dock;
						dragged_window_dock->redock_from = this;
						dragged_window_dock->redock_to = container->active_dock;
						dragged_window_dock->redock_slot = dock_slot;
						dragged_window_dock->redock_from_window = owner;
					}
					else dragged_window->get_dockspace().node.splits[0]->active_dock->redock_to = nullptr;
				}

			}

		}
		else
		{
			ImVec2 calculated_size0 = size, calculated_size1;
			auto& split0 = container->splits[0];
			auto& split1 = container->splits[1];

			if (split0 && split1)
			{
				float acontsizeX = split0->size ? split0->size :
					split1->size ? size.x - split1->size - splitter_button_width : size.x / 2 - splitterButtonWidthHalf;
				float acontsizeY = split0->size ? split0->size :
					split1->size ? size.y - split1->size - splitter_button_width : size.y / 2 - splitterButtonWidthHalf;

				float bcontsizeX = split0->size ? size.x - split0->size - splitter_button_width :
					split1->size ? split1->size : size.x / 2 - splitterButtonWidthHalf;
				float bcontsizeY = split0->size ? size.y - split0->size - splitter_button_width :
					split1->size ? split1->size : size.y / 2 - splitterButtonWidthHalf;

				calculated_size0 = ImVec2(container->vertical_split ? acontsizeX : size.x, !container->vertical_split ? acontsizeY : size.y);
				calculated_size1 = ImVec2(container->vertical_split ? bcontsizeX : size.x, !container->vertical_split ? bcontsizeY : size.y);
			}
			if (split0)
			{
				if (split0 == nullptr)
					size.x = 1;
				render_container(idgen, split0, calculated_size0, calculated_cursor_pos);
				if (container->vertical_split)
				{
					calculated_cursor_pos.x = calculated_size0.x + calculated_cursor_pos.x + splitter_button_width;
				}
				else
				{
					calculated_cursor_pos.y = calculated_size0.y + calculated_cursor_pos.y + splitter_button_width;
				}
			}
			if (split1)
			{
				ImGui::SetCursorPosX(calculated_cursor_pos.x - splitter_button_width);
				ImGui::SetCursorPosY(calculated_cursor_pos.y - splitter_button_width);
				std::string idnamesb = "##SplitterButton";
				idnamesb += idgen++;


				ImVec4 button_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
				ImVec4 button_active_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
				ImVec4 button_hovered_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
				button_color.w = 0.0f;
				button_active_color.w = 0.0f;
				button_hovered_color.w = 0.0f;

				ImGui::PushStyleColor(ImGuiCol_Button, button_color);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_active_color);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hovered_color);

				ImGui::Button(idnamesb.c_str(), ImVec2(
					container->vertical_split ? splitter_button_width : size.x + splitter_button_width,
					!container->vertical_split ? splitter_button_width : size.y + splitter_button_width));

				ImGui::PopStyleColor(3);
				ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

				if (ImGui::IsItemActive())
				{
					float mouse_delta = !container->vertical_split ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
					if (mouse_delta != 0.0f)
					{
						if (split0 && split0->always_auto_resize != true)
						{
							ImVec2 minSize;
							get_min_size(split0, minSize);
							if (split0->size == 0)
								split0->size = container->vertical_split ? calculated_size1.x : calculated_size1.y;
							if (split0->size + mouse_delta >= (container->vertical_split ? minSize.x : minSize.y))
								split0->size += mouse_delta;

							if (split0->size + mouse_delta >= (container->vertical_split ? size.x : size.y))
								split0->size = (container->vertical_split ? size.x : size.y);
						}
						else
						{
							ImVec2 minSize;
							get_min_size(split1, minSize);
							if (split1->size == 0)
								split1->size = container->vertical_split ? calculated_size1.x : calculated_size1.y;
							if (split1->size - mouse_delta >= (container->vertical_split ? minSize.x : minSize.y))
								split1->size -= mouse_delta;

							if (split1->size - mouse_delta >= (container->vertical_split ? size.x : size.y))
								split1->size = (container->vertical_split ? size.x : size.y);
						}
					}
				}

				if (ImGui::IsItemHovered() || ImGui::IsItemActive())
					ImGui::SetMouseCursor(container->vertical_split ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);

				render_container(idgen, split1, calculated_size1, calculated_cursor_pos);
			}
		}
	};

	void Dockspace::update_and_draw(ImVec2 dockspaceSize)
	{
		uint32_t idgen = 0;
		ImVec2 backup_pos = ImGui::GetCursorPos();
		render_container(idgen, &node, dockspaceSize, backup_pos);
		ImGui::SetCursorPos(backup_pos);
		if (_current_dock_to)
		{
			if (_current_dock_action == eUndock)
			{
				if (undock(_current_dock_to))
				{
					_current_dock_to->container = nullptr;
					_current_dock_to->draging = true;
					
					auto guiWindow = std::make_shared<GuiWindow>(
						sf::VideoMode((unsigned int)_current_dock_to->last_size.x, (unsigned int)_current_dock_to->last_size.y),
						"",//std::string(m_currentDockTo->title),
						sf::Style::Default
						);

					auto engine = core::get_subsystem<runtime::Engine>();
					engine->register_window(guiWindow);
					guiWindow->get_dockspace().dock(_current_dock_to, DockSlot::Tab, 0, true);
					guiWindow->setPosition(sf::Mouse::getPosition());
					guiWindow->requestFocus();
				}
			}
			else if (_current_dock_action == eDrag)
			{
				if (!_current_dock_to->draging)
				{

				}
				_current_dock_to->draging = true;
			}
			else if (_current_dock_action == eClose)
			{
				if (_current_dock_to->on_close_func)
				{
					if (_current_dock_to->on_close_func()) undock(_current_dock_to);
				}
				else undock(_current_dock_to);
			}
			_current_dock_to = nullptr;
			_current_dock_action = eNull;
		}

		updateDrag(owner);
	}

	void Dockspace::clear()
	{
		for (auto container : nodes)
		{
			delete container;
		}
		nodes.clear();

		node = {};
	}

	bool Dockspace::has_dock(const std::string& name)
	{
		for (auto n : nodes)
		{
			if (n->active_dock)
			{
				if (n->active_dock->title == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	GuiWindow *Dockspace::is_any_window_dragged()
	{
		auto engine = core::get_subsystem<runtime::Engine>();
		const auto& windows = engine->get_windows();

		for (auto window : windows)
		{
			GuiWindow* guiWindow = static_cast<GuiWindow*>(window.get());
			auto& dockspace = guiWindow->get_dockspace();
			if (dockspace.node.splits[0] && dockspace.node.splits[0]->active_dock)
			{
				if (dockspace.node.splits[0]->active_dock->draging)
					return guiWindow;
			}
			
		}
		

		return nullptr;
	}

	static ImRect getSlotRect(ImRect parent_rect, DockSlot dock_slot)
	{
		ImVec2 size = parent_rect.Max - parent_rect.Min;
		ImVec2 center = parent_rect.Min + size * 0.5f;
		switch (dock_slot)
		{
		default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
		case DockSlot::Top: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
		case DockSlot::Right: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
		case DockSlot::Bottom: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
		case DockSlot::Left: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
		}
	}


	DockSlot Dockspace::render_dock_slot_preview(const ImVec2& mouse_pos, const ImVec2& cPos, const ImVec2& cSize)
	{
		DockSlot dock_slot = DockSlot::None;

		ImRect rect{ cPos, cPos + cSize };
	
		auto checkSlot = [&mouse_pos](ImRect rect, DockSlot slot) -> bool
		{

			auto slotRect = getSlotRect(rect, slot);

			ImGui::GetWindowDrawList()->AddRectFilled(
				slotRect.Min,
				slotRect.Max,
				ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.8f)));//tab


			return slotRect.Contains(mouse_pos);

		};

		if (checkSlot(rect, DockSlot::Tab))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dock_slot = DockSlot::Tab;
		}

		if (checkSlot(rect, DockSlot::Left))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dock_slot = DockSlot::Left;
		}

		if (checkSlot(rect, DockSlot::Right))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dock_slot = DockSlot::Right;
		}
			

		if (checkSlot(rect, DockSlot::Top))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + (cSize.y / 2.0f)), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dock_slot = DockSlot::Top;
		}
			

		if (checkSlot(rect, DockSlot::Bottom))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x, cPos.y + (cSize.y / 2.0f)), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dock_slot = DockSlot::Bottom;
		}
			
		return dock_slot;
	}

	bool Dockspace::get_min_size(Node* container, ImVec2& min)
	{
		if (container->splits[0] == nullptr)
		{
			if (min.x < container->active_dock->min_size.x)
				min.x = container->active_dock->min_size.x;
			if (min.y < container->active_dock->min_size.y)
				min.y = container->active_dock->min_size.y;
			return true;
		}
		else
		{
			if (get_min_size(container->splits[0], min))
			{
				if (container->splits[1])
				{
					if (get_min_size(container->splits[1], min))
					{
						return true;
					}
				}
			};
		}

		return false;
	}

	void Dockspace::render_tab_bar(Node* container, const ImVec2& size, const ImVec2& cursor_pos, float tabbar_height)
	{
		ImGui::SetCursorPos(cursor_pos);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
		ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

		for (auto dock : container->docks)
		{
			std::string dockTitle = dock->title;
			if (dock->close_button == true)
				dockTitle += "  ";


			if (dock == container->active_dock)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, childBg);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
			}
			else
			{
				ImVec4 button_color = ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w);
				ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
				ImVec4 buttonColorHovered = ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);
				ImGui::PushStyleColor(ImGuiCol_Button, button_color);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
			}
			if (ImGui::Button(dockTitle.c_str(), ImVec2(0, tabbar_height)))
			{
				container->active_dock = dock;
			}
			if (ImGui::IsItemActive())
			{
				float delta = ImGui::GetCursorScreenPos().y - ImGui::GetIO().MousePos.y;
				if (dock->container->parent == &node && node.splits[1] == nullptr && dock->container->docks.size() == 1)
				{
					if (delta < -2 || delta > 2)
					{
						_current_dock_action = eDrag;
						_current_dock_to = dock;
					}
				}
				else
				{
					if (delta < -4 || delta > 27)
					{
						if (dock->undockable == false)
						{
							_current_dock_action = eUndock;
							_current_dock_to = dock;
						}
					}
				}
			}


			ImGui::SameLine();
// 			if (dock->closeButton == true)
// 			{
// 				ImVec2 backupCursorPos = ImGui::GetCursorScreenPos();
// 				ImGui::SetCursorPosX(backupCursorPos.x - 26);
// 				ImGui::SetCursorPosY(backupCursorPos.y + 10);
// 				ImGui::SetItemAllowOverlap();
// 				if(ImGui::CloseButton(5, ImVec2(backupCursorPos.x - 18, backupCursorPos.y + 10), 7))
// 				{
// 					m_currentDockToAction = eClose;
// 					m_currentDockTo = dock;
// 				}
// 				ImGui::SetCursorPos(backupCursorPos);
// 			}

			ImGui::PopStyleColor(3);
		}
		ImGui::PopStyleVar();
	}
}