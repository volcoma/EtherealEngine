#define IMGUI_DEFINE_MATH_OPERATORS
#include "imguidock.h"

// user includes
#include "core/string_utils/string_utils.h"
#include "docking.h"
#include "runtime/input/input.h"
#include "runtime/rendering/renderer.h"

namespace imguidock
{
dockspace::dockspace()
{
}

dockspace::~dockspace()
{
	clear();
}

bool dockspace::dock_to(dock* ddock, slot dock_slot, float size, bool active)
{
	return dock_with(ddock, nullptr, dock_slot, size, active);
}

bool dockspace::dock_with(dock* ddock, dock* dock_to, slot dock_slot, float size, bool active)
{
	if(ddock == nullptr)
		return false;

	node* current_container = &root;

	if(dock_to != nullptr)
	{
		if(dock_slot == imguidock::slot::tab)
		{
			dock_to->container->active_dock = active ? ddock
													 : current_container->splits[0]->active_dock
														   ? current_container->splits[0]->active_dock
														   : ddock;
			dock_to->container->docks.push_back(ddock);
			ddock->container = dock_to->container;
			return true;
		}
		else
		{
			nodes.push_back(new node{});
			auto new_container = nodes[nodes.size() - 1];
			new_container->parent = dock_to->container->parent;
			new_container->splits[0] = dock_to->container;
			new_container->size = dock_to->container->size;
			// if (size)
			//	new_container->alwaysAutoResize = false;
			dock_to->container->size = 0;
			if(dock_to->container->parent->splits[0] == dock_to->container)
				dock_to->container->parent->splits[0] = new_container;
			else
				dock_to->container->parent->splits[1] = new_container;
			// dock_to->container->parent = new_container;
			dock_to->container = new_container->splits[0];
			dock_to->container->parent = new_container;
			current_container = new_container;
		}
	}

	node* child_container = nullptr;
	if(current_container->splits[0] == nullptr || current_container->splits[1] == nullptr)
	{
		nodes.push_back(new node{});
		child_container = nodes[nodes.size() - 1];
	};

	if(current_container->splits[0] == nullptr)
	{
		current_container->splits[0] = child_container;
		current_container->splits[0]->active_dock = active ? ddock
														   : current_container->splits[0]->active_dock
																 ? current_container->splits[0]->active_dock
																 : ddock;
		current_container->splits[0]->docks.push_back(ddock);
		current_container->splits[0]->parent = current_container;
		current_container->splits[0]->size = size < 0 ? size * -1.0f : size;
		ddock->container = current_container->splits[0];
		ddock->container->parent = current_container;
	}
	else if(current_container->splits[1] == nullptr)
	{
		current_container->splits[1] = child_container;
		// node *otherSplit = current_container->splits[0];
		if(size > 0.0f)
		{
			current_container->splits[0]->always_auto_resize = true;
			current_container->splits[0]->size = 0.0f;
			current_container->splits[1]->size = size;
			current_container->splits[1]->always_auto_resize = false;
		}
		else if(size == 0.0f)
		{
		}
		else
		{
			current_container->splits[0]->always_auto_resize = false;
			current_container->splits[0]->size = size * -1.0f;
			current_container->splits[1]->size = 0.0f;
			current_container->splits[1]->always_auto_resize = true;
		}
		switch(dock_slot)
		{
			case imguidock::slot::left:
				current_container->splits[1] = current_container->splits[0];
				current_container->splits[0] = child_container;
				current_container->vertical_split = true;
				break;
			case imguidock::slot::right:
				current_container->vertical_split = true;
				break;
			case imguidock::slot::top:
				current_container->splits[1] = current_container->splits[0];
				current_container->splits[0] = child_container;
				current_container->vertical_split = false;
				break;
			case imguidock::slot::bottom:
				current_container->vertical_split = false;
				break;
			case imguidock::slot::tab:
				current_container->vertical_split = false;
				break;
			case imguidock::slot::none:
				break;
		}
		child_container->active_dock =
			active ? ddock : child_container->active_dock ? child_container->active_dock : ddock;
		child_container->docks.push_back(ddock);
		child_container->parent = current_container;

		//	if (child_container->parent != nullptr && current_container->verticalSplit !=
		// child_container->parent->verticalSplit)
		//		current_container->size = otherSplit->size ? otherSplit->size + otherSplit->size :
		// otherSplit->size;

		ddock->container = child_container;
	}
	else
	{
		return false;
	}

	return true;
}

bool dockspace::undock(dock* dock)
{
	if(dock != nullptr)
	{
		if(dock->container->docks.size() > 1)
		{
			for(size_t i = 0; i < dock->container->docks.size(); ++i)
			{
				if(dock->container->docks[i] == dock)
				{
					dock->last_size = dock->container->active_dock->last_size;
					dock->container->docks.erase(dock->container->docks.begin() + static_cast<int>(i));
					if(i != dock->container->docks.size())
						dock->container->active_dock = dock->container->docks[i];
					else
						dock->container->active_dock = dock->container->docks[i - 1];
				}
			}
		}
		else
		{
			node *to_delete = nullptr, *parent_to_delete = nullptr;
			if(dock->container->parent == &root)
			{
				if(root.splits[0] == dock->container)
				{
					if(root.splits[1])
					{
						to_delete = root.splits[0];
						if(root.splits[1]->splits[0])
						{
							parent_to_delete = root.splits[1];
							root.splits[0] = root.splits[1]->splits[0];
							root.splits[0]->parent = &root;
							root.splits[0]->vertical_split = false;
							root.splits[1] = root.splits[1]->splits[1];
							root.splits[1]->parent = &root;
							root.splits[1]->parent->vertical_split = root.splits[1]->vertical_split;
							root.splits[1]->vertical_split = false;
						}
						else
						{
							root.splits[0] = root.splits[1];
							root.splits[1] = nullptr;
							root.splits[0]->size = 0;
							root.splits[0]->vertical_split = false;
							root.splits[0]->parent->vertical_split = false;
						}
					}
					else
						return false;
				}
				else
				{
					to_delete = root.splits[1];
					root.splits[1] = nullptr;
				}
			}
			else
			{
				parent_to_delete = dock->container->parent;
				if(dock->container->parent->splits[0] == dock->container)
				{
					to_delete = dock->container->parent->splits[0];
					node* parent = dock->container->parent->parent;
					node* working = nullptr;
					if(dock->container->parent->parent->splits[0] == dock->container->parent)
						working = dock->container->parent->parent->splits[0] =
							dock->container->parent->splits[1];
					else
						working = dock->container->parent->parent->splits[1] =
							dock->container->parent->splits[1];
					working->parent = parent;
					working->size = dock->container->parent->size;
				}
				else
				{
					to_delete = dock->container->parent->splits[1];
					node* parent = dock->container->parent->parent;
					node* working = nullptr;
					if(dock->container->parent->parent->splits[0] == dock->container->parent)
						working = dock->container->parent->parent->splits[0] =
							dock->container->parent->splits[0];
					else
						working = dock->container->parent->parent->splits[1] =
							dock->container->parent->splits[0];
					working->parent = parent;
					working->size = dock->container->parent->size;
				}
			}
			for(size_t i = 0; i < nodes.size(); ++i)
			{
				if(to_delete == nodes[i])
				{
					delete nodes[i];
					nodes.erase(nodes.begin() + static_cast<int>(i));
					break;
				}
				if(nodes.size() > 1 && parent_to_delete == nodes[i])
				{
					delete nodes[i];
					nodes.erase(nodes.begin() + static_cast<int>(i));
					break;
				}
				if(nodes.size() > 1 && to_delete == nodes[i])
				{
					delete nodes[i];
					nodes.erase(nodes.begin() + static_cast<int>(i));
					break;
				}
			}
		}
		return true;
	}
	return false;
}

void update_drag(std::uint32_t window_id)
{
	auto& renderer = core::get_subsystem<runtime::renderer>();
	auto& docking = core::get_subsystem<docking_system>();
	auto& dockspace = docking.get_dockspace(window_id);
	auto& window = renderer.get_window(window_id);
	auto& split = dockspace.root.splits[0];

	if(split && split->active_dock)
	{
		auto& active_dock = split->active_dock;
		bool mouseleft = mml::mouse::is_button_pressed(mml::mouse::left);
		if(active_dock->draging && mouseleft)
		{
			auto pos = mml::mouse::get_position();
			pos[0] -= 40;
			pos[1] -= 30 + int32_t(ImGui::GetTextLineHeightWithSpacing());
			if(active_dock->redock_to == nullptr || active_dock->redock_slot == slot::none)
			{
				window->set_position(pos);
				window->set_size({{640, 480}});
			}
			window->set_opacity(0.3f);
			active_dock->redock_to = nullptr;
		}
		else
		{
			if(active_dock->draging)
			{
				if(active_dock->redock_to)
				{
					active_dock->container = nullptr;
					active_dock->redock_from->dock_with(active_dock, active_dock->redock_to,
														active_dock->redock_slot, 0, true);
					active_dock->redock_to = nullptr;
					active_dock->redock_slot = slot::none;
					window->request_close();
				}
				else
				{
					window->set_opacity(1.0f);
				}
			}
			active_dock->draging = false;
		}
	}
}

void dockspace::render_container(uint32_t& idgen, node* container, ImVec2 size, ImVec2 cursor_pos)
{
	const float tabbar_height = ImGui::GetItemsLineHeightWithSpacing();
	ImVec2 calculated_size = size;
	ImVec2 calculated_cursor_pos = cursor_pos;

	idgen++;

	std::string idname = "Dock##";
	idname += std::to_string(idgen);

	calculated_size.y -= tabbar_height;

	float splitter_button_width = 4;
	float splitterButtonWidthHalf = splitter_button_width / 2;

	if(container->splits[0] == nullptr && container != &root)
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

		std::uint32_t dragged_window_id = is_any_window_dragged();
		if(dragged_window_id != INVALID_WINDOW)
		{
			auto& renderer = core::get_subsystem<runtime::renderer>();
			auto& docking = core::get_subsystem<docking_system>();
			auto& dockspace = docking.get_dockspace(dragged_window_id);
			if(dockspace.root.splits[0]->active_dock != container->active_dock)
			{
				auto& owner = renderer.get_window(owner_id);

				auto mouse_pos = mml::mouse::get_position(*owner);
				ImVec2 cursor_pos = {float(mouse_pos[0]), float(mouse_pos[1])};
				if((mouse_pos[0] > screen_cursor_pos.x && mouse_pos[0] < (screen_cursor_pos.x + size.x)) &&
				   (mouse_pos[1] > screen_cursor_pos.y && mouse_pos[1] < (screen_cursor_pos.y + size.y)))
				{
					ImGui::BeginChild("##dockSlotPreview");
					ImGui::PushClipRect(ImVec2(), ImGui::GetIO().DisplaySize, false);
					slot dock_slot =
						render_dock_slot_preview(dragged_window_id, cursor_pos, screen_cursor_pos, size);
					ImGui::PopClipRect();
					ImGui::EndChild();
					auto dragged_window_dock = dockspace.root.splits[0]->active_dock;
					if(dragged_window_dock)
					{
						if(dock_slot != slot::none)
						{
							dragged_window_dock->redock_from = this;
							dragged_window_dock->redock_to = container->active_dock;
							dragged_window_dock->redock_from_window = owner_id;
						}
						dragged_window_dock->redock_slot = dock_slot;
					}
				}
			}
		}
	}
	else
	{
		ImVec2 calculated_size0 = size, calculated_size1;
		auto& split0 = container->splits[0];
		auto& split1 = container->splits[1];

		if(split0 && split1)
		{
			float acontsizeX = split0->size != 0.0f
								   ? split0->size
								   : split1->size != 0.0f ? size.x - split1->size - splitter_button_width
														  : size.x / 2 - splitterButtonWidthHalf;
			float acontsizeY = split0->size != 0.0f
								   ? split0->size
								   : split1->size != 0.0f ? size.y - split1->size - splitter_button_width
														  : size.y / 2 - splitterButtonWidthHalf;

			float bcontsizeX =
				split0->size != 0.0f
					? size.x - split0->size - splitter_button_width
					: split1->size != 0.0f ? split1->size : size.x / 2 - splitterButtonWidthHalf;
			float bcontsizeY =
				split0->size != 0.0f
					? size.y - split0->size - splitter_button_width
					: split1->size != 0.0f ? split1->size : size.y / 2 - splitterButtonWidthHalf;

			calculated_size0 = ImVec2(container->vertical_split ? acontsizeX : size.x,
									  !container->vertical_split ? acontsizeY : size.y);
			calculated_size1 = ImVec2(container->vertical_split ? bcontsizeX : size.x,
									  !container->vertical_split ? bcontsizeY : size.y);
		}
		if(split0)
		{
			
			render_container(idgen, split0, calculated_size0, calculated_cursor_pos);
			if(container->vertical_split)
			{
				calculated_cursor_pos.x =
					calculated_size0.x + calculated_cursor_pos.x + splitter_button_width;
			}
			else
			{
				calculated_cursor_pos.y =
					calculated_size0.y + calculated_cursor_pos.y + splitter_button_width;
			}
		}
		if(split1)
		{
            if(split0 == nullptr)
				size.x = 1;
			ImGui::SetCursorPosX(calculated_cursor_pos.x - splitter_button_width);
			ImGui::SetCursorPosY(calculated_cursor_pos.y - splitter_button_width);
			std::string idnamesb = "##SplitterButton";
			idnamesb += std::to_string(idgen++);

			ImVec4 button_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
			ImVec4 button_active_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
			ImVec4 button_hovered_color = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
			button_color.w = 0.0f;
			button_active_color.w = 0.0f;
			button_hovered_color.w = 0.0f;

			ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_active_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hovered_color);

			ImGui::Button(
				idnamesb.c_str(),
				ImVec2(container->vertical_split ? splitter_button_width : size.x + splitter_button_width,
					   !container->vertical_split ? splitter_button_width : size.y + splitter_button_width));

			ImGui::PopStyleColor(3);
			ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter.

			if(ImGui::IsItemActive())
			{
				float mouse_delta =
					!container->vertical_split ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
				if(mouse_delta != 0.0f)
				{
					if(split0 && split0->always_auto_resize != true)
					{
						ImVec2 minSize;
						get_min_size(split0, minSize);
						if(split0->size == 0.0f)
							split0->size =
								container->vertical_split ? calculated_size1.x : calculated_size1.y;
						if(split0->size + mouse_delta >= (container->vertical_split ? minSize.x : minSize.y))
							split0->size += mouse_delta;

						if(split0->size + mouse_delta >= (container->vertical_split ? size.x : size.y))
							split0->size = (container->vertical_split ? size.x : size.y);
					}
					else
					{
						ImVec2 minSize;
						get_min_size(split1, minSize);
						if(split1->size == 0.0f)
							split1->size =
								container->vertical_split ? calculated_size1.x : calculated_size1.y;
						if(split1->size - mouse_delta >= (container->vertical_split ? minSize.x : minSize.y))
							split1->size -= mouse_delta;

						if(split1->size - mouse_delta >= (container->vertical_split ? size.x : size.y))
							split1->size = (container->vertical_split ? size.x : size.y);
					}
				}
			}

			if(ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(container->vertical_split ? ImGuiMouseCursor_ResizeEW
																: ImGuiMouseCursor_ResizeNS);

			render_container(idgen, split1, calculated_size1, calculated_cursor_pos);
		}
	}
};

void dockspace::update_and_draw(ImVec2 dockspaceSize)
{
	uint32_t idgen = 0;
	ImVec2 backup_pos = ImGui::GetCursorPos();

	render_container(idgen, &root, dockspaceSize, backup_pos);
	if(_current_dock_to)
	{
		if(_current_dock_action == eUndock)
		{
			if(undock(_current_dock_to))
			{
				_current_dock_to->container = nullptr;
				_current_dock_to->draging = true;

				auto& renderer = core::get_subsystem<runtime::renderer>();
				auto& docking = core::get_subsystem<docking_system>();
				auto window = std::make_unique<render_window>(
					mml::video_mode(static_cast<unsigned int>(_current_dock_to->last_size.x),
									static_cast<unsigned int>(_current_dock_to->last_size.y)),
					"Window", mml::style::standard);

				auto& dockspace = docking.get_dockspace(window->get_id());
				dockspace.dock_to(_current_dock_to, slot::tab, 0, true);
				auto pos = mml::mouse::get_position();
				pos[0] -= 40;
				pos[1] -= 30 + int32_t(ImGui::GetTextLineHeightWithSpacing());

				window->set_position(pos);
				window->request_focus();
				renderer.register_window(std::move(window));
			}
		}
		else if(_current_dock_action == eDrag)
		{
			if(!_current_dock_to->draging)
			{
			}
			_current_dock_to->draging = true;
		}
		else if(_current_dock_action == eClose)
		{
			if(_current_dock_to->on_close_func)
			{
				if(_current_dock_to->on_close_func())
					undock(_current_dock_to);
			}
			else
				undock(_current_dock_to);
		}
		_current_dock_to = nullptr;
		_current_dock_action = eNull;
	}
	update_drag(owner_id);
}

void dockspace::clear()
{
	for(auto container : nodes)
	{
		delete container;
	}
	nodes.clear();

	root = {};
}

void activete_dock_impl(node* node, const std::string& title)
{
	for(auto d : node->docks)
	{
		if(string_utils::begins_with(d->title, title))
		{
			node->active_dock = d;
			return;
		}
	}

	if(node->splits[0])
		activete_dock_impl(node->splits[0], title);

	if(node->splits[1])
		activete_dock_impl(node->splits[1], title);
}

void dockspace::activate_dock(const std::string& name)
{
	activete_dock_impl(&root, name);
}

std::uint32_t dockspace::is_any_window_dragged()
{
	auto& docking = core::get_subsystem<docking_system>();
	const auto& dockspaces = docking.get_dockspaces();

	for(const auto& p : dockspaces)
	{
		auto id = p.first;
		auto& dockspace = p.second;
		if(dockspace.root.splits[0] && dockspace.root.splits[0]->active_dock)
		{
			if(dockspace.root.splits[0]->active_dock->draging)
				return id;
		}
	}

	return INVALID_WINDOW;
}

static bool is_point_in_convex(ImVec2 point, const std::vector<ImVec2>& convex)
{

	if(convex.empty())
		return false;

	// n>2 Keep track of cross product sign changes
	float pos = 0;
	float neg = 0;

	for(size_t i = 0; i < convex.size(); i++)
	{

		// Form a segment between the i'th point
		float x1 = convex[i].x;
		float y1 = convex[i].y;

		// And the i+1'th, or if i is the last, with the first point
		auto i2 = i < convex.size() - 1 ? i + 1 : 0;

		float x2 = convex[i2].x;
		float y2 = convex[i2].y;

		float x = point.x;
		float y = point.y;

		// Compute the cross product
		float d = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);

		if(d > 0)
			pos++;
		if(d < 0)
			neg++;

		// If the sign changes, then point is outside
		if(pos > 0 && neg > 0)
			return false;
	}

	// If no change in direction, then on same side of all segments, and thus inside
	return true;
}

static ImRect get_slot_rect(ImRect parent_rect, slot dock_slot)
{
	ImVec2 size = parent_rect.Max - parent_rect.Min;
	ImVec2 center = parent_rect.Min + size * 0.5f;
	switch(dock_slot)
	{
		default:
			return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
		case slot::top:
			return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
		case slot::right:
			return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
		case slot::bottom:
			return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
		case slot::left:
			return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
	}
}

static void get_slot_convex(ImRect parent_rect, slot dock_slot, std::vector<ImVec2>& convex)
{
	convex.clear();
	ImVec2 size = parent_rect.Max - parent_rect.Min;
	const float tabbar_height = ImGui::GetItemsLineHeightWithSpacing();
	ImVec2 top_left_corner = parent_rect.Min;
	ImVec2 bottom_right_corner = parent_rect.Max;
	ImVec2 top_right_corner = ImVec2(parent_rect.Max.x, parent_rect.Min.y);
	ImVec2 bottom_left_corner = ImVec2(parent_rect.Min.x, parent_rect.Max.y);
	ImVec2 sz = size * 0.18f;
	float offset = 0.0f;

	switch(dock_slot)
	{
		default:
		{
			auto top_left_offset = top_left_corner + ImVec2(offset, 0.0f);
			auto top_right_offset = top_right_corner - ImVec2(offset, 0.0f);
			auto bottom_right_offset = top_right_corner + ImVec2(offset, tabbar_height);
			auto bottom_left_offset = top_left_corner + ImVec2(offset, tabbar_height);

			convex.emplace_back(ImVec2(top_left_offset.x, top_left_offset.y));
			convex.emplace_back(ImVec2(top_right_offset.x, top_right_offset.y));
			convex.emplace_back(ImVec2(bottom_right_offset.x, bottom_right_offset.y));
			convex.emplace_back(ImVec2(bottom_left_offset.x, bottom_left_offset.y));

			return;
		}
		case slot::top:
		{
			auto top_left_offset = top_left_corner + ImVec2(offset, tabbar_height);
			auto top_right_offset = top_right_corner - ImVec2(offset, -tabbar_height);
			convex.emplace_back(top_left_offset);
			convex.emplace_back(top_right_offset);
			convex.emplace_back(ImVec2(top_right_offset.x - sz.x, top_right_offset.y + sz.y));
			convex.emplace_back(ImVec2(top_left_offset.x + sz.x, top_left_offset.y + sz.y));
			return;
		}
		case slot::right:
		{
			auto bottom_right_offset = bottom_right_corner - ImVec2(0.0f, offset);
			auto top_right_offset = top_right_corner + ImVec2(0.0f, offset + tabbar_height);

			convex.emplace_back(bottom_right_offset);
			convex.emplace_back(ImVec2(bottom_right_offset.x - sz.x, bottom_right_offset.y - sz.y));
			convex.emplace_back(ImVec2(top_right_offset.x - sz.x, top_right_offset.y + sz.y));
			convex.emplace_back(top_right_offset);
			return;
		}
		case slot::bottom:
		{
			auto bottom_left_offset = bottom_left_corner + ImVec2(offset, 0.0f);
			auto bottom_right_offset = bottom_right_corner - ImVec2(offset, 0.0f);

			convex.emplace_back(bottom_left_offset);
			convex.emplace_back(ImVec2(bottom_left_offset.x + sz.x, bottom_left_offset.y - sz.y));
			convex.emplace_back(ImVec2(bottom_right_offset.x - sz.x, bottom_right_offset.y - sz.y));
			convex.emplace_back(bottom_right_offset);
			return;
		}
		case slot::left:
		{
			auto top_left_offset = top_left_corner + ImVec2(0.0f, offset + tabbar_height);
			auto bottom_left_offset = bottom_left_corner - ImVec2(0.0f, offset);

			convex.emplace_back(top_left_offset);
			convex.emplace_back(ImVec2(top_left_offset.x + sz.x, top_left_offset.y + sz.y));
			convex.emplace_back(ImVec2(bottom_left_offset.x + sz.x, bottom_left_offset.y - sz.y));
			convex.emplace_back(bottom_left_offset);
			return;
		}
	}
}

slot dockspace::render_dock_slot_preview(std::uint32_t id, const ImVec2& mouse_pos, const ImVec2& cPos,
										 const ImVec2& cSize)
{
	bool convex_slot_preview = true;

	slot dock_slot = slot::none;

	ImRect rect{cPos, cPos + cSize};
	ImVec4 col = {1.0f, 1.0f, 1.0f, 0.8f};
	ImVec4 col_preview = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
	col_preview.w = 0.3f;
	auto check_slot = [convex_slot_preview, &col, &mouse_pos](ImRect rect, slot slot,
															  std::vector<ImVec2>& convex) -> bool {

		if(convex_slot_preview)
		{
			get_slot_convex(rect, slot, convex);
			if(!convex.empty())
			{
				ImGui::GetWindowDrawList()->AddPolyline(convex.data(), 4, ImGui::ColorConvertFloat4ToU32(col),
														true, 2.0f, true);
			}

			return is_point_in_convex(mouse_pos, convex);
		}
		else
		{
			auto slot_rect = get_slot_rect(rect, slot);
			ImGui::GetWindowDrawList()->AddRectFilled(slot_rect.Min, slot_rect.Max,
													  ImGui::ColorConvertFloat4ToU32(col));

			return slot_rect.Contains(mouse_pos);
		}

	};

	ImVec2 offset = ImGui::GetStyle().FramePadding * 2.0f;
	std::vector<ImVec2> convex;
	ImRect preview_rect = rect;
	if(check_slot(rect, slot::tab, convex))
	{
		preview_rect = {cPos, ImVec2(cPos.x + cSize.x, cPos.y + cSize.y)};
		dock_slot = slot::tab;
	}

	if(check_slot(rect, slot::left, convex))
	{
		preview_rect = {cPos, ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y + cSize.y)};
		dock_slot = slot::left;
	}

	if(check_slot(rect, slot::right, convex))
	{

		preview_rect = {ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y),
						ImVec2(cPos.x + cSize.x, cPos.y + cSize.y)};
		dock_slot = slot::right;
	}

	if(check_slot(rect, slot::top, convex))
	{
		preview_rect = {cPos, ImVec2(cPos.x + cSize.x, cPos.y + (cSize.y / 2.0f))};
		dock_slot = slot::top;
	}

	if(check_slot(rect, slot::bottom, convex))
	{
		preview_rect = {ImVec2(cPos.x, cPos.y + (cSize.y / 2.0f)),
						ImVec2(cPos.x + cSize.x, cPos.y + cSize.y)};
		dock_slot = slot::bottom;
	}

	if(dock_slot != slot::none)
	{
		auto& renderer = core::get_subsystem<runtime::renderer>();
		auto& owner = renderer.get_window(owner_id);
		auto& wnd = renderer.get_window(id);

		std::array<int32_t, 2> pos;
		pos[0] = int32_t(preview_rect.Min.x - offset.x) + owner->get_position()[0];
		pos[1] = int32_t(preview_rect.Min.y - offset.y) + owner->get_position()[1];
		wnd->set_position(pos);
		wnd->set_size(
			{{uint32_t(preview_rect.GetSize().x + offset.x), uint32_t(preview_rect.GetSize().y + offset.y)}});
		ImGui::GetWindowDrawList()->AddRectFilled(preview_rect.Min, preview_rect.Max,
												  ImGui::ColorConvertFloat4ToU32(col_preview), false, 0);
	}

	return dock_slot;
}

bool dockspace::get_min_size(node* container, ImVec2& min)
{
	if(container->splits[0] == nullptr)
	{
		if(min.x < container->active_dock->min_size.x)
			min.x = container->active_dock->min_size.x;
		if(min.y < container->active_dock->min_size.y)
			min.y = container->active_dock->min_size.y;
		return true;
	}
	else
	{
		if(get_min_size(container->splits[0], min))
		{
			if(container->splits[1])
			{
				if(get_min_size(container->splits[1], min))
				{
					return true;
				}
			}
		};
	}

	return false;
}

void dockspace::render_tab_bar(node* container, const ImVec2&, const ImVec2& cursor_pos, float tabbar_height)
{
	ImGui::SetCursorPos(cursor_pos);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
	ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

	for(auto dock : container->docks)
	{
		std::string dockTitle = dock->title;

		ImVec4 button_color =
			ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w * 0.6f);
		ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
		ImVec4 buttonColorHovered =
			ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);

		bool is_dock_active = dock == container->active_dock;
		if(is_dock_active)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
		}

		const auto& style = ImGui::GetStyle();

		const ImVec2 title_label_size = ImGui::CalcTextSize(dockTitle.c_str(), NULL, true);
		ImVec2 title_sz =
			ImGui::CalcItemSize(ImVec2(0, tabbar_height), title_label_size.x + style.FramePadding.x * 2.0f,
								title_label_size.y + style.FramePadding.y * 2.0f);

		const ImVec2 close_label_size = ImGui::CalcTextSize("X", NULL, true);
		ImVec2 close_sz =
			ImGui::CalcItemSize(ImVec2(0, tabbar_height), close_label_size.x + style.FramePadding.x * 2.0f,
								close_label_size.y + style.FramePadding.y * 2.0f);

		if(ImGui::Button(dockTitle.c_str(), ImVec2(title_sz.x + close_sz.x * 1.5f, tabbar_height)))
		{
			container->active_dock = dock;
			is_dock_active = true;
		}

		bool is_button_active = ImGui::IsItemActive();
		if(is_button_active)
		{
			if(dock->container->parent == &root && root.splits[1] == nullptr &&
			   dock->container->docks.size() == 1)
			{
				if(ImGui::IsMouseDragging(0, 4.0f))
				{
					_current_dock_action = eDrag;
					_current_dock_to = dock;
				}
			}
			else
			{
				if(ImGui::IsMouseDragging(0, 8.0f))
				{
					if(dock->undockable == false)
					{
						_current_dock_action = eUndock;
						_current_dock_to = dock;
					}
				}
			}
		}
		ImGui::PopStyleColor(3);
		bool is_hovered = ImGui::IsItemHoveredRect();

		if((is_hovered || is_dock_active) && dock->close_button == true)
		{
			if(is_dock_active)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, childBg);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button,
									  is_button_active ? buttonColorActive : buttonColorHovered);
			}

			ImGui::SameLine(0, 0);
			ImGui::SetItemAllowOverlap();
			ImVec2 backupCursorPos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorPosX(backupCursorPos.x - close_sz.x);
			ImGui::PushID(dockTitle.c_str());
			if(ImGui::Button("X", ImVec2(0, tabbar_height)))
			{
				_current_dock_action = eClose;
				_current_dock_to = dock;
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
		}

		ImGui::SameLine();
	}
	ImGui::PopStyleVar();
}

void dock::initialize(const std::string& dtitle, bool close_btn, const ImVec2& min_sz,
					  std::function<void(const ImVec2&)> ddrawFunction)
{
	static int i = 0;
	title = dtitle + "###" + std::to_string(i++);
	close_button = close_btn;
	min_size = min_sz;
	draw_function = ddrawFunction;
}
}
