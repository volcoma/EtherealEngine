#include "docks.h"
#include "../../edit_state.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/systems/transform_system.h"
#include "runtime/input/input.h"
#include "runtime/system/filesystem.h"
#include "runtime/rendering/mesh.h"
namespace Docks
{

	void checkContextMenu(core::Entity entity)
	{
		auto es = core::get_subsystem<EditState>();
		auto ecs = core::get_subsystem<core::EntityComponentSystem>();
		auto& editorCamera = es->camera;
		if (entity && entity != editorCamera)
		{
			if (gui::BeginPopupContextItem("Entity Context Menu"))
			{
				if (gui::Selectable("Create child"))
				{
					auto object = ecs->create();
					object.assign<TransformComponent>().lock()
						->setParent(entity.component<TransformComponent>());
				}
				if (gui::Selectable("Clone"))
				{
					auto object = ecs->create_from_copy(entity);
					object.component<TransformComponent>().lock()
						->setParent(entity.component<TransformComponent>().lock()->getParent(), false, true);
					
					es->select(object);
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
					auto object = ecs->create();
					object.assign<TransformComponent>();
				}

				gui::EndPopup();
			}
		}


	}

	void checkDrag(core::Entity entity, bool isHovered)
	{
		auto es = core::get_subsystem<EditState>();
		auto& editorCamera = es->camera;
		auto& dragged = es->dragData.object;

		auto ecs = core::get_subsystem<core::EntityComponentSystem>();

		if (entity)
		{
			if (isHovered)
			{
				if (gui::IsMouseClicked(2) && entity != editorCamera)
				{
					es->drag(entity, entity.to_string());
				}
				if (gui::IsMouseReleased(2))
				{
					if (dragged && entity != editorCamera)
					{
						if (dragged.is_type<core::Entity>())
						{
							
							auto draggedEntity = dragged.get_value<core::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());

							es->drop();
						}

						if (dragged.is_type<AssetHandle<Prefab>>())
						{
							auto prefab = dragged.get_value<AssetHandle<Prefab>>();
							auto draggedEntity = prefab->instantiate();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());

							es->drop();
							es->select(draggedEntity);
						}
						if (dragged.is_type<AssetHandle<Mesh>>())
						{
							auto mesh = dragged.get_value<AssetHandle<Mesh>>();
							Model model;		
							model.setLod(mesh, 0);

							auto object = ecs->create();
							//Add component and configure it.
							object.assign<TransformComponent>().lock()
								->setParent(entity.component<TransformComponent>());
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->setCastShadow(true)
								.setCastReflelction(false)
								.setModel(model);

							es->drop();
							es->select(object);
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
						if (dragged.is_type<core::Entity>())
						{
							auto draggedEntity = dragged.get_value<core::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->setParent(core::CHandle<TransformComponent>());

							es->drop();
						}
						if (dragged.is_type<AssetHandle<Prefab>>())
						{
							auto prefab = dragged.get_value<AssetHandle<Prefab>>();
							auto draggedEntity = prefab->instantiate();
							es->drop();
							es->select(draggedEntity);
							
						}
						if (dragged.is_type<AssetHandle<Mesh>>())
						{
							auto mesh = dragged.get_value<AssetHandle<Mesh>>();
							Model model;
							model.setLod(mesh, 0);

							auto object = ecs->create();
							//Add component and configure it.
							object.assign<TransformComponent>();
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->setCastShadow(true)
								.setCastReflelction(false)
								.setModel(model);

							es->drop();
							es->select(object);
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

	void drawEntity(core::Entity entity)
	{
		if (!entity)
			return;

		gui::PushID(entity.id().index());
		gui::AlignFirstTextHeightToWidgets();
		auto es = core::get_subsystem<EditState>();
		auto& selected = es->selectionData.object;

		bool isSselected = false;
		if (selected && selected.is_type<core::Entity>())
		{
			isSselected = selected.get_value<core::Entity>() == entity;
		}	
		
		bool isParentOfSelected = false;
// 		if (selected && selected.is_type<core::Entity>())
// 		{
// 			auto selectedEntity = selected.get_value<core::Entity>();
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
			es->select(entity);
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
		auto es = core::get_subsystem<EditState>();
		auto ecs = core::get_subsystem<core::EntityComponentSystem>();
		auto ts = core::get_subsystem<TransformSystem>();
		auto is = core::get_subsystem<InputSystem>();

		auto& input = is->get_context();
		auto& roots = ts->get_roots();

		auto& editorCamera = es->camera;
		auto& selected = es->selectionData.object;

		checkContextMenu(core::Entity());

		if (gui::IsWindowFocused())
		{
			if (input.isKeyPressed(sf::Keyboard::Delete))
			{
				if (selected && selected.is_type<core::Entity>())
				{
					auto sel = selected.get_value<core::Entity>();
					if (sel != editorCamera)
					{
						sel.destroy();
						es->unselect();
					}
				}
			}

			if (input.isKeyPressed(sf::Keyboard::D))
			{
				if (input.isKeyDown(sf::Keyboard::LControl))
				{
					if (selected && selected.is_type<core::Entity>())
					{
						auto sel = selected.get_value<core::Entity>();
						if (sel != editorCamera)
						{
							auto clone = ecs->create_from_copy(sel);
							clone.component<TransformComponent>().lock()
								->setParent(sel.component<TransformComponent>().lock()->getParent(), false, true);
							es->select(clone);
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