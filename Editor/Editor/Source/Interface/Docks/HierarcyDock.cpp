#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/Ecs/Prefab.h"
#include "Runtime/Ecs/Utils.h"
#include "Runtime/Ecs/Components/TransformComponent.h"
#include "Runtime/Ecs/Systems/TransformSystem.h"
#include "Runtime/Input/InputContext.h"
#include "Runtime/System/FileSystem.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Ecs/Components/ModelComponent.h"
namespace Docks
{

	void checkContextMenu(ecs::Entity entity)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& world = app.getWorld();
		auto& editState = app.getEditState();
		auto& editorCamera = editState.camera;
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
					auto object = world.entities.create_from_copy(entity);
					object.component<TransformComponent>().lock()
						->setParent(entity.component<TransformComponent>().lock()->getParent(), false, true);
					
					editState.select(object);
				}
				if (gui::Selectable("Save As Prefab"))
				{
					ecs::utils::saveEntity("data://prefabs/", entity);
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

	void checkDrag(ecs::Entity entity, bool isHovered)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		auto& world = app.getWorld();
		auto& editorCamera = editState.camera;
		auto& dragged = editState.dragData.object;
		if (entity)
		{
			if (isHovered)
			{
				if (gui::IsMouseClicked(2) && entity != editorCamera)
				{
					editState.drag(entity, entity.to_string());
				}
				if (gui::IsMouseReleased(2))
				{
					if (dragged && entity != editorCamera)
					{
						if (dragged.is_type<ecs::Entity>())
						{
							auto draggedEntity = dragged.get_value<ecs::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());

							editState.drop();
						}

						if (dragged.is_type<AssetHandle<Prefab>>())
						{
							auto prefab = dragged.get_value<AssetHandle<Prefab>>();
							auto draggedEntity = prefab->instantiate();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());

							editState.drop();
							editState.select(draggedEntity);
						}
						if (dragged.is_type<AssetHandle<Mesh>>())
						{
							auto mesh = dragged.get_value<AssetHandle<Mesh>>();
							Model model;		
							model.setLod(mesh, 0);

							auto object = world.entities.create();
							//Add component and configure it.
							object.assign<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->setCastShadow(true)
								.setCastReflelction(false)
								.setModel(model);

							editState.drop();
							editState.select(object);
						}
					}
				}
			}
			else
			{
				if (dragged && gui::IsWindowHovered() && !gui::IsAnyItemHovered())
				{
					if (gui::IsMouseReleased(2))
					{
						if (dragged.is_type<ecs::Entity>())
						{
							auto draggedEntity = dragged.get_value<ecs::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(ecs::ComponentHandle<TransformComponent>());

							editState.drop();
						}
						if (dragged.is_type<AssetHandle<Prefab>>())
						{
							auto prefab = dragged.get_value<AssetHandle<Prefab>>();
							auto draggedEntity = prefab->instantiate();
							editState.drop();
							editState.select(draggedEntity);
							
						}
						if (dragged.is_type<AssetHandle<Mesh>>())
						{
							auto mesh = dragged.get_value<AssetHandle<Mesh>>();
							Model model;
							model.setLod(mesh, 0);

							auto object = world.entities.create();
							//Add component and configure it.
							object.assign<TransformComponent>();
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->setCastShadow(true)
								.setCastReflelction(false)
								.setModel(model);

							editState.drop();
							editState.select(object);
						}
					}
				}
			}
		}
		
	}

	bool isParentOf(TransformComponent* trans, TransformComponent* selectedTransform)
	{
		if (!selectedTransform)
			return false;

		if (trans == selectedTransform)
		{
			return true;
		}
		auto parent = selectedTransform->getParent().lock().get();
		return isParentOf(trans, parent);
	};

	void drawEntity(ecs::Entity entity)
	{
		if (!entity)
			return;

		gui::PushID(entity.id().index());
		gui::AlignFirstTextHeightToWidgets();
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		auto& selected = editState.selectionData.object;

		bool isSselected = false;
		if (selected && selected.is_type<ecs::Entity>())
		{
			isSselected = selected.get_value<ecs::Entity>() == entity;
		}	
		
		bool isParentOfSelected = false;
// 		if (selected && selected.is_type<ecs::Entity>())
// 		{
// 			auto selectedEntity = selected.get_value<ecs::Entity>();
// 			auto selectedTransformComponent = selectedEntity.component<TransformComponent>().lock()->getParent().lock().get();
// 			auto entityTransformComponent = entity.component<TransformComponent>().lock().get();
// 			isParentOfSelected = isParentOf(entityTransformComponent, selectedTransformComponent);
// 		}	
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

		if(isParentOfSelected)
			gui::SetNextTreeNodeOpen(true);

		bool opened = gui::TreeNodeEx(name.c_str(), flags);
		bool hovered = gui::IsItemHovered();
		if (gui::IsItemClicked(0))
		{
			editState.select(entity);
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
					if (!child.expired())
						drawEntity(child.lock()->getEntity());
				}
			}

			gui::TreePop();
		}

		gui::PopID();
	}

	void renderHierarcy(ImVec2 area)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& world = app.getWorld();
		auto& editState = app.getEditState();
		auto system = world.systems.system<TransformSystem>();
		auto& roots = system->getRoots();

		auto& editorCamera = editState.camera;
		auto& selected = editState.selectionData.object;

		checkContextMenu(ecs::Entity());

		if (gui::IsWindowFocused())
		{
			auto& input = app.getInput();

			if (input.isKeyPressed(sf::Keyboard::Delete))
			{
				if (selected && selected.is_type<ecs::Entity>())
				{
					auto sel = selected.get_value<ecs::Entity>();
					if (sel != editorCamera)
					{
						sel.destroy();
						editState.unselect();
					}
				}
			}

			if (input.isKeyPressed(sf::Keyboard::D))
			{
				if (input.isKeyDown(sf::Keyboard::LControl))
				{
					if (selected && selected.is_type<ecs::Entity>())
					{
						auto sel = selected.get_value<ecs::Entity>();
						if (sel != editorCamera)
						{
							auto clone = world.entities.create_from_copy(sel);
							clone.component<TransformComponent>().lock()
								->setParent(sel.component<TransformComponent>().lock()->getParent(), false, true);
							editState.select(clone);
						}
					}
				}
			}
		}

		for (auto& root : roots)
		{
			if (!root.expired())
				drawEntity(root.lock()->getEntity());
		}		
	}

};