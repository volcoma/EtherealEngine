#include "HierarchyView.h"
#include "Runtime/runtime.h"
#include "../EditorApp.h"

HierarchyView::HierarchyView()
{
	mName = "Hierarchy";
}


HierarchyView::~HierarchyView()
{
}

void HierarchyView::render(AppWindow& window)
{
	ImGuiIO& io = gui::GetIO();
	ImVec2 sz = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
	gui::SetNextWindowPos(ImVec2(sz.x - sz.x/2, sz.y - sz.y / 2));
	gui::SetNextWindowSize(sz, ImGuiSetCond_FirstUseEver);
	if (!gui::BeginDock(mName.c_str(), &mOpen))
	{
		gui::EndDock();
		return;
	}
	
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	auto system = world.systems.system<TransformSystem>();
	auto& roots = system->getRoots();

	auto& editorCamera = editState.editorCamera;
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	auto& dragged = selection.dragged;

	checkContextMenu(ecs::Entity());
	
	if (gui::IsWindowFocused())
	{
		auto& input = app.getInputManager();

		if (input.isKeyPressed(sf::Keyboard::Delete))
		{
			if(selected && selected != editorCamera)
				selected.destroy();
		}

		if (input.isKeyPressed(sf::Keyboard::D))
		{
			if (input.isKeyDown(sf::Keyboard::LControl))
			{
				if (selected && selected != editorCamera)
				{
					auto clone = world.entities.create_from_copy(selected);
					clone.component<TransformComponent>().lock()
						->setParent(selected.component<TransformComponent>().lock()->getParent(), false, true);
					selected = clone;
				}
			}	
		}		
	}

	if (dragged)
		gui::SetTooltip(dragged.to_string().c_str());


	for (auto& root : roots)
	{
		if(!root.expired())
			drawEntity(root.lock()->getEntity());
	}

	if (gui::IsWindowHovered() && !gui::IsAnyItemHovered())
	{
		if (gui::IsMouseClicked(0))
		{
			selected = ecs::Entity();
			dragged = ecs::Entity();
		}
		if (gui::IsMouseReleased(2))
		{
			if (dragged)
			{
				dragged.component<TransformComponent>().lock()
 					->setParent(ecs::ComponentHandle<TransformComponent>());
				dragged = ecs::Entity();
			}
			
		}
	}

	gui::EndDock();
}

void HierarchyView::drawEntity(ecs::Entity entity)
{
	gui::PushID(entity.id().index());
	gui::AlignFirstTextHeightToWidgets();
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;

	bool isSselected = (selected && selected == entity);
	bool noChildren = true;
	std::string name = entity.to_string();
	

	ImGuiTreeNodeFlags flags = 0
	| ImGuiTreeNodeFlags_OpenOnDoubleClick
	| ImGuiTreeNodeFlags_OpenOnArrow;

	if (isSselected)
		flags |= ImGuiTreeNodeFlags_Selected;


	auto transformComponent = entity.component<TransformComponent>().lock();
	if (transformComponent)
	{
		noChildren = transformComponent->getChildren().empty();
	}
	
	if (noChildren)
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool opened = gui::TreeNodeEx(name.c_str(), flags);
	bool hovered = gui::IsItemHovered();
	if(gui::IsItemClicked(0))
	{
		selected = entity;
	}

	checkContextMenu(entity);
	checkDrag(entity, hovered);
	if (opened)
	{
		if (!noChildren)
		{
			auto children = entity.component<TransformComponent>().lock()->getChildren();
			for (auto& child : children)
			{
				if(!child.expired())
					drawEntity(child.lock()->getEntity());
			}
		}
		
		gui::TreePop();
	}

	gui::PopID();
}

void HierarchyView::checkContextMenu(ecs::Entity entity)
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	auto& editorCamera = editState.editorCamera;
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	if (entity && entity != editorCamera)
	{
		if (gui::BeginPopupContextItem("Entity Context Menu"))
		{
			if (gui::Selectable("Create child"))
			{			
				auto object = world.entities.create();
				object.assign<TransformComponent>().lock()
					->setParent(entity.component<TransformComponent>());
			}
			if (gui::Selectable("Clone"))
			{
				selected = world.entities.create_from_copy(entity);
				selected.component<TransformComponent>().lock()
					->setParent(entity.component<TransformComponent>().lock()->getParent(), false, true);
			}
			if (gui::Selectable("Delete"))
			{			
				entity.destroy();
			}

			gui::EndPopup();
		}
	}
	else
	{
		if (gui::BeginPopupContextWindow())
		{
			if (gui::Selectable("Create empty"))
			{
				auto object = world.entities.create();
				object.assign<TransformComponent>();
			}

			gui::EndPopup();
		}
	}

	
}

void HierarchyView::checkDrag(ecs::Entity entity, bool isHovered)
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& editorCamera = editState.editorCamera;
	auto& selection = editState.entitySelection;
	auto& dragged = selection.dragged;
	if (entity)
	{
		if (isHovered)
		{
			auto rounding = gui::GetStyle().FrameRounding;
			float thickness = 2.0f;
			gui::RenderFrameEx(gui::GetItemRectMin(), ImVec2(gui::GetContentRegionMax().x, gui::GetItemRectMax().y), true, rounding, thickness);
			if (gui::IsMouseClicked(2) && entity != editorCamera)
			{
				dragged = entity;
			}
			if (gui::IsMouseReleased(2))
			{
				if (dragged && dragged != entity && entity != editorCamera)
				{
					dragged.component<TransformComponent>().lock()
						->setParent(entity.component<TransformComponent>());
					dragged = ecs::Entity();
				}
				else
				{
					dragged = ecs::Entity();
				}
			}
		}		
	}
}