#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGuiDock.h"
#include "../GuiWindow.h"
#include "../../EditorApp.h"

namespace ImGuiDock
{
	Dockspace::Dockspace(GuiWindow* owner) : mOwner(owner)
	{

	}

	Dockspace::~Dockspace()
	{
		clear();
	}

	bool Dockspace::dock(Dock* dock, DockSlot dockSlot, float size, bool active)
	{
		return dockWith(dock, nullptr, dockSlot, size, active);
	}

	bool Dockspace::dockWith(Dock* dock, Dock* dockTo, DockSlot dockSlot, float size, bool active)
	{
		if (dock == nullptr)
			return false;

		Node* currentContainer = &node;

		if (dockTo != nullptr)
		{
			if (dockSlot == ImGuiDock::DockSlot::Tab)
			{
				dockTo->container->activeDock = active ? dock : currentContainer->splits[0]->activeDock ? currentContainer->splits[0]->activeDock : dock;
				dockTo->container->docks.push_back(dock);
				dock->container = dockTo->container;
				return true;
			}
			else
			{
				nodes.push_back(new Node{});
				auto newContainer = nodes[nodes.size() - 1];
				newContainer->parent = dockTo->container->parent;
				newContainer->splits[0] = dockTo->container;
				newContainer->size = dockTo->container->size;
				//if (size)
				//	newContainer->alwaysAutoResize = false;
				dockTo->container->size = 0;
				if (dockTo->container->parent->splits[0] == dockTo->container)
					dockTo->container->parent->splits[0] = newContainer;
				else dockTo->container->parent->splits[1] = newContainer;
				//dockTo->container->parent = newContainer;
				dockTo->container = newContainer->splits[0];
				dockTo->container->parent = newContainer;
				currentContainer = newContainer;
			}
		}

		Node *childContainer = nullptr;
		if (currentContainer->splits[0] == nullptr || currentContainer->splits[1] == nullptr)
		{
			nodes.push_back(new Node{});
			childContainer = nodes[nodes.size() - 1];
		};

		if (currentContainer->splits[0] == nullptr)
		{
			currentContainer->splits[0] = childContainer;
			currentContainer->splits[0]->activeDock = active ? dock : currentContainer->splits[0]->activeDock ? currentContainer->splits[0]->activeDock : dock;
			currentContainer->splits[0]->docks.push_back(dock);
			currentContainer->splits[0]->parent = currentContainer;
			currentContainer->splits[0]->size = size < 0 ? size * -1 : size;
			dock->container = currentContainer->splits[0];
			dock->container->parent = currentContainer;
		}
		else if (currentContainer->splits[1] == nullptr)
		{
			currentContainer->splits[1] = childContainer;
			//Node *otherSplit = currentContainer->splits[0];
			if (size > 0)
			{
				currentContainer->splits[0]->alwaysAutoResize = true;
				currentContainer->splits[0]->size = 0;
				currentContainer->splits[1]->size = size;
				currentContainer->splits[1]->alwaysAutoResize = false;
			}
			else if (size == 0) {}
			else
			{
				currentContainer->splits[0]->alwaysAutoResize = false;
				currentContainer->splits[0]->size = size * -1;
				currentContainer->splits[1]->size = 0;
				currentContainer->splits[1]->alwaysAutoResize = true;
			}
			switch (dockSlot)
			{
			case ImGuiDock::DockSlot::Left:
				currentContainer->splits[1] = currentContainer->splits[0];
				currentContainer->splits[0] = childContainer;
				currentContainer->verticalSplit = true;
				break;
			case ImGuiDock::DockSlot::Right:
				currentContainer->verticalSplit = true;
				break;
			case ImGuiDock::DockSlot::Top:
				currentContainer->splits[1] = currentContainer->splits[0];
				currentContainer->splits[0] = childContainer;
				currentContainer->verticalSplit = false;
				break;
			case ImGuiDock::DockSlot::Bottom:
				currentContainer->verticalSplit = false;
				break;
			case ImGuiDock::DockSlot::Tab:
				currentContainer->verticalSplit = false;
				break;
			case ImGuiDock::DockSlot::None:
				break;
			default:
				break;
			}
			childContainer->activeDock = active ? dock : childContainer->activeDock ? childContainer->activeDock : dock;
			childContainer->docks.push_back(dock);
			childContainer->parent = currentContainer;

			//	if (childContainer->parent != nullptr && currentContainer->verticalSplit != childContainer->parent->verticalSplit)
			//		currentContainer->size = otherSplit->size ? otherSplit->size + otherSplit->size : otherSplit->size;

			dock->container = childContainer;
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
				for (size_t i = 0; i < dock->container->docks.size(); i++)
				{
					if (dock->container->docks[i] == dock)
					{
						dock->lastSize = dock->container->activeDock->lastSize;
						dock->container->docks.erase(dock->container->docks.begin() + i);
						if (i != dock->container->docks.size())
							dock->container->activeDock = dock->container->docks[i];
						else dock->container->activeDock = dock->container->docks[i - 1];
					}
				}
			}
			else
			{
				Node *toDelete = nullptr, *parentToDelete = nullptr;
				if (dock->container->parent == &node)
				{
					if (node.splits[0] == dock->container)
					{
						if (node.splits[1])
						{
							toDelete = node.splits[0];
							if (node.splits[1]->splits[0]) {
								parentToDelete = node.splits[1];
								node.splits[0] = node.splits[1]->splits[0];
								node.splits[0]->parent = &node;
								node.splits[0]->verticalSplit = false;
								node.splits[1] = node.splits[1]->splits[1];
								node.splits[1]->parent = &node;
								node.splits[1]->parent->verticalSplit = node.splits[1]->verticalSplit;
								node.splits[1]->verticalSplit = false;
							}
							else
							{
								node.splits[0] = node.splits[1];
								node.splits[1] = nullptr;
								node.splits[0]->size = 0;
								node.splits[0]->verticalSplit = false;
								node.splits[0]->parent->verticalSplit = false;
							}
						}
						else return false;
					}
					else
					{
						toDelete = node.splits[1];
						node.splits[1] = nullptr;
					}
				}
				else
				{
					parentToDelete = dock->container->parent;
					if (dock->container->parent->splits[0] == dock->container)
					{
						toDelete = dock->container->parent->splits[0];
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
						toDelete = dock->container->parent->splits[1];
						Node *parent = dock->container->parent->parent;
						Node *working = nullptr;
						if (dock->container->parent->parent->splits[0] == dock->container->parent)
							working = dock->container->parent->parent->splits[0] = dock->container->parent->splits[0];
						else working = dock->container->parent->parent->splits[1] = dock->container->parent->splits[0];
						working->parent = parent;
						working->size = dock->container->parent->size;
					}
				}
				for (size_t i = 0; i < nodes.size(); i++)
				{
					if (toDelete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
					}
					if (nodes.size() > 1 && parentToDelete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
					}
					if (nodes.size() > 1 && toDelete == nodes[i])
					{
						delete nodes[i];
						nodes.erase(nodes.begin() + i);
					}
				}
			}
			return true;
		}
		return false;
	}

	void updateDrag(GuiWindow* window)
	{
		auto& dockspace = window->getDockspace();
		auto& split = dockspace.node.splits[0];

		if (split && split->activeDock)
		{
			auto& activeDock = split->activeDock;
			bool mouseleft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
			if (activeDock->draging && mouseleft)
			{
				auto pos = sf::Mouse::getPosition();
				pos.x -= 20;
				pos.y -= 40;
				window->setPosition(pos);
				window->setAlpha(0.3f);
			}
			else
			{
				if (activeDock->draging)
				{
					if (activeDock->redockTo)
					{
						activeDock->container = nullptr;
						activeDock->redockFrom->dockWith(
							activeDock,
							activeDock->redockTo,
							activeDock->redockSlot, 0, true);
						activeDock->redockTo = nullptr;

						window->close();
					}
					else
					{
						window->setAlpha(1.0f);
					}
				}
				activeDock->draging = false;
			}
		}
	}

	void Dockspace::updateAndDraw(ImVec2 dockspaceSize)
	{
		uint32_t idgen = 0;

		float tabbarHeight = 20;

		std::function<void(Node*, ImVec2, ImVec2)> renderContainer = [&](Node *container, ImVec2 size, ImVec2 cursorPos) {
			ImVec2 calculatedSize = size;
			ImVec2 calculatedCursorPos = cursorPos;

			idgen++;

			std::string idname = "Dock##";
			idname += idgen;

			calculatedSize.y -= tabbarHeight;

			float splitterButtonWidth = 4;
			float splitterButtonWidthHalf = splitterButtonWidth / 2;

			if (container->splits[0] == nullptr && container != &node)
			{
				_renderTabBar(container, calculatedSize, cursorPos);
				cursorPos.y += tabbarHeight;

				ImGui::SetCursorPos(cursorPos);
				ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
				screenCursorPos.y -= tabbarHeight;

				ImGui::BeginChild(idname.c_str(), calculatedSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				container->activeDock->drawFunction(calculatedSize);
				container->activeDock->lastSize = calculatedSize;

				ImGui::EndChild();

				GuiWindow* draggedWindow = _isAnyWindowDragging();
				if (draggedWindow != nullptr && draggedWindow->getDockspace().node.splits[0]->activeDock != container->activeDock)
				{
					auto mousePos = sf::Mouse::getPosition(*mOwner);
					ImVec2 cursorPos = { float(mousePos.x), float(mousePos.y) };
					if ((mousePos.x > screenCursorPos.x && mousePos.x < (screenCursorPos.x + size.x)) &&
						(mousePos.y > screenCursorPos.y && mousePos.y < (screenCursorPos.y + size.y)))
					{

						ImGui::BeginChild("##dockSlotPreview");
						ImGui::PushClipRect(ImVec2(), ImGui::GetIO().DisplaySize, false);
						DockSlot dockSlot = _renderDockSlotPreview(cursorPos, screenCursorPos, size);
						ImGui::PopClipRect();
						ImGui::EndChild();
						if (dockSlot != DockSlot::None)
						{
							Dock *draggedWindowDock = draggedWindow->getDockspace().node.splits[0]->activeDock;
							draggedWindowDock->redockFrom = this;
							draggedWindowDock->redockTo = container->activeDock;
							draggedWindowDock->redockSlot = dockSlot;
							draggedWindowDock->redockFromWindow = mOwner;
						}
						else draggedWindow->getDockspace().node.splits[0]->activeDock->redockTo = nullptr;
					}

				}

			}
			else
			{
				ImVec2 calculatedSize0 = size, calculatedSize1;
				auto& split0 = container->splits[0];
				auto& split1 = container->splits[1];

				if (split0 && split1)
				{
					float acontsizeX = split0->size ? split0->size :
						split1->size ? size.x - split1->size - splitterButtonWidth : size.x / 2 - splitterButtonWidthHalf;
					float acontsizeY = split0->size ? split0->size :
						split1->size ? size.y - split1->size - splitterButtonWidth : size.y / 2 - splitterButtonWidthHalf;

					float bcontsizeX = split0->size ? size.x - split0->size - splitterButtonWidth :
						split1->size ? split1->size : size.x / 2 - splitterButtonWidthHalf;
					float bcontsizeY = split0->size ? size.y - split0->size - splitterButtonWidth :
						split1->size ? split1->size : size.y / 2 - splitterButtonWidthHalf;

					calculatedSize0 = ImVec2(container->verticalSplit ? acontsizeX : size.x, !container->verticalSplit ? acontsizeY : size.y);
					calculatedSize1 = ImVec2(container->verticalSplit ? bcontsizeX : size.x, !container->verticalSplit ? bcontsizeY : size.y);
				}
				if (split0)
				{
					if (split0 == nullptr)
						size.x = 1;
					renderContainer(split0, calculatedSize0, calculatedCursorPos);
					if (container->verticalSplit)
						calculatedCursorPos.x = calculatedSize0.x + calculatedCursorPos.x + splitterButtonWidth;
					else
					{
						calculatedCursorPos.y = calculatedSize0.y + calculatedCursorPos.y + splitterButtonWidth;
					}
				}
				if (split1)
				{
					ImGui::SetCursorPosX(calculatedCursorPos.x - splitterButtonWidth);
					ImGui::SetCursorPosY(calculatedCursorPos.y - splitterButtonWidth);
					std::string idnamesb = "##SplitterButton";
					idnamesb += idgen++;


					ImVec4 buttonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
					ImVec4 buttonActiveColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
					ImVec4 buttonHoveredColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
					buttonColor.w = 0.0f;
					buttonActiveColor.w = 0.0f;
					buttonHoveredColor.w = 0.0f;

					ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);

					ImGui::Button(idnamesb.c_str(), ImVec2(
						container->verticalSplit ? splitterButtonWidth : size.x + splitterButtonWidth,
						!container->verticalSplit ? splitterButtonWidth : size.y + splitterButtonWidth));

					ImGui::PopStyleColor(3);
					ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

					if (ImGui::IsItemActive())
					{
						float mouse_delta = !container->verticalSplit ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
						if (mouse_delta != 0.0f)
						{
							if (split0 && split0->alwaysAutoResize != true)
							{
								ImVec2 minSize;
								_getMinSize(split0, minSize);
								if (split0->size == 0)
									split0->size = container->verticalSplit ? calculatedSize1.x : calculatedSize1.y;
								if (split0->size + mouse_delta >= (container->verticalSplit ? minSize.x : minSize.y))
									split0->size += mouse_delta;

								if (split0->size + mouse_delta >= (container->verticalSplit ? size.x : size.y))
									split0->size = (container->verticalSplit ? size.x : size.y);
							}
							else
							{
								ImVec2 minSize;
								_getMinSize(split1, minSize);
								if (split1->size == 0)
									split1->size = container->verticalSplit ? calculatedSize1.x : calculatedSize1.y;
								if (split1->size - mouse_delta >= (container->verticalSplit ? minSize.x : minSize.y))
									split1->size -= mouse_delta;

								if (split1->size - mouse_delta >= (container->verticalSplit ? size.x : size.y))
									split1->size = (container->verticalSplit ? size.x : size.y);
							}
						}
					}

					if (ImGui::IsItemHovered() || ImGui::IsItemActive())
						ImGui::SetMouseCursor(container->verticalSplit ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);

					renderContainer(split1, calculatedSize1, calculatedCursorPos);
				}
			}
		};

		ImVec2 backup_pos = ImGui::GetCursorPos();
		renderContainer(&node, dockspaceSize, backup_pos);
		ImGui::SetCursorPos(backup_pos);
		if (mCurrentDockTo)
		{
			if (mCurrentDockAction == eUndock)
			{
				if (undock(mCurrentDockTo))
				{
					mCurrentDockTo->container = nullptr;
					mCurrentDockTo->draging = true;
					
					auto guiWindow = std::make_shared<GuiWindow>(
						sf::VideoMode((unsigned int)mCurrentDockTo->lastSize.x, (unsigned int)mCurrentDockTo->lastSize.y),
						"",//std::string(m_currentDockTo->title),
						sf::Style::Resize
						);
					Singleton<EditorApp>::getInstance().registerWindow(guiWindow);
					guiWindow->getDockspace().dock(mCurrentDockTo, DockSlot::Tab, 0, true);
					guiWindow->setPosition(sf::Mouse::getPosition());
					guiWindow->requestFocus();
				}
			}
			else if (mCurrentDockAction == eDrag)
			{
				if (!mCurrentDockTo->draging)
				{

				}
				mCurrentDockTo->draging = true;
			}
			else if (mCurrentDockAction == eClose)
			{
				if (mCurrentDockTo->onCloseFunction)
				{
					if (mCurrentDockTo->onCloseFunction()) undock(mCurrentDockTo);
				}
				else undock(mCurrentDockTo);
			}
			mCurrentDockTo = nullptr;
			mCurrentDockAction = eNull;
		}

		updateDrag(mOwner);
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

	bool Dockspace::hasDock(const std::string& name)
	{
		for (auto n : nodes)
		{
			if (n->activeDock)
			{
				if (n->activeDock->title == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	GuiWindow *Dockspace::_isAnyWindowDragging()
	{
		auto windows = Singleton<EditorApp>::getInstance().getWindows();

		for (auto window : windows)
		{
			GuiWindow* guiWindow = static_cast<GuiWindow*>(window.get());
			auto& dockspace = guiWindow->getDockspace();
			if (dockspace.node.splits[0] && dockspace.node.splits[0]->activeDock)
			{
				if (dockspace.node.splits[0]->activeDock->draging)
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


	DockSlot Dockspace::_renderDockSlotPreview(const ImVec2& mousePos, const ImVec2& cPos, const ImVec2& cSize)
	{
		DockSlot dockSlot = DockSlot::None;

		ImRect rect{ cPos, cPos + cSize };
	
		auto checkSlot = [&mousePos](ImRect rect, DockSlot slot) -> bool
		{

			auto slotRect = getSlotRect(rect, slot);

			ImGui::GetWindowDrawList()->AddRectFilled(
				slotRect.Min,
				slotRect.Max,
				ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.8f)));//tab


			return slotRect.Contains(mousePos);

		};

		if (checkSlot(rect, DockSlot::Tab))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dockSlot = DockSlot::Tab;
		}

		if (checkSlot(rect, DockSlot::Left))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dockSlot = DockSlot::Left;
		}

		if (checkSlot(rect, DockSlot::Right))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dockSlot = DockSlot::Right;
		}
			

		if (checkSlot(rect, DockSlot::Top))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + (cSize.y / 2.0f)), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dockSlot = DockSlot::Top;
		}
			

		if (checkSlot(rect, DockSlot::Bottom))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x, cPos.y + (cSize.y / 2.0f)), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
			dockSlot = DockSlot::Bottom;
		}
			
		return dockSlot;
	}

	bool Dockspace::_getMinSize(Node* container, ImVec2& min)
	{
		if (container->splits[0] == nullptr)
		{
			if (min.x < container->activeDock->minSize.x)
				min.x = container->activeDock->minSize.x;
			if (min.y < container->activeDock->minSize.y)
				min.y = container->activeDock->minSize.y;
			return true;
		}
		else
		{
			if (_getMinSize(container->splits[0], min))
			{
				if (container->splits[1])
				{
					if (_getMinSize(container->splits[1], min))
					{
						return true;
					}
				}
			};
		}

		return false;
	}

	void Dockspace::_renderTabBar(Node* container, const ImVec2& size, const ImVec2& cursorPos)
	{
		ImGui::SetCursorPos(cursorPos);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
		ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

		for (auto dock : container->docks)
		{
			std::string dockTitle = dock->title;
			if (dock->closeButton == true)
				dockTitle += "  ";


			if (dock == container->activeDock)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, childBg);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
			}
			else
			{
				ImVec4 buttonColor = ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w);
				ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
				ImVec4 buttonColorHovered = ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);
				ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
			}
			if (ImGui::Button(dockTitle.c_str(), ImVec2(0, 20)))
			{
				container->activeDock = dock;
			}
			if (ImGui::IsItemActive())
			{
				float delta = ImGui::GetCursorScreenPos().y - ImGui::GetIO().MousePos.y;
				if (dock->container->parent == &node && node.splits[1] == nullptr && dock->container->docks.size() == 1)
				{
					if (delta < -2 || delta > 2)
					{
						mCurrentDockAction = eDrag;
						mCurrentDockTo = dock;
					}
				}
				else
				{
					if (delta < -4 || delta > 27)
					{
						if (dock->undockable == false)
						{
							mCurrentDockAction = eUndock;
							mCurrentDockTo = dock;
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