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

	void check_context_menu(runtime::Entity entity)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		auto& editorCamera = es->camera;
		if (entity && entity != editorCamera)
		{
			if (gui::BeginPopupContextItem("Entity Context Menu"))
			{
				if (gui::Selectable("Create child"))
				{
					auto object = ecs->create();
					object.assign<TransformComponent>().lock()
						->set_parent(entity.component<TransformComponent>());
				}
				if (gui::Selectable("Clone"))
				{
					auto object = ecs->create_from_copy(entity);
					object.component<TransformComponent>().lock()
						->set_parent(entity.component<TransformComponent>().lock()->get_parent(), false, true);
					
					es->select(object);
				}
				if (gui::Selectable("Save As Prefab"))
				{
					ecs::utils::save_entity("data://prefabs/", entity);
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

	void check_drag(runtime::Entity entity, bool isHovered)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto& editorCamera = es->camera;
		auto& dragged = es->drag_data.object;

		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

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
						if (dragged.is_type<runtime::Entity>())
						{
							
							auto draggedEntity = dragged.get_value<runtime::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->set_parent(entity.component<TransformComponent>());

							es->drop();
						}

						if (dragged.is_type<AssetHandle<Prefab>>())
						{
							auto prefab = dragged.get_value<AssetHandle<Prefab>>();
							auto draggedEntity = prefab->instantiate();
							draggedEntity.component<TransformComponent>().lock()
								->set_parent(entity.component<TransformComponent>());

							es->drop();
							es->select(draggedEntity);
						}
						if (dragged.is_type<AssetHandle<Mesh>>())
						{
							auto mesh = dragged.get_value<AssetHandle<Mesh>>();
							Model model;		
							model.set_lod(mesh, 0);

							auto object = ecs->create();
							//Add component and configure it.
							object.assign<TransformComponent>().lock()
								->set_parent(entity.component<TransformComponent>());
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->set_casts_shadow(true)
								.set_casts_reflection(false)
								.set_model(model);

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
						if (dragged.is_type<runtime::Entity>())
						{
							auto draggedEntity = dragged.get_value<runtime::Entity>();
							draggedEntity.component<TransformComponent>().lock()
								->set_parent(runtime::CHandle<TransformComponent>());

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
							model.set_lod(mesh, 0);

							auto object = ecs->create();
							//Add component and configure it.
							object.assign<TransformComponent>();
							//Add component and configure it.
							object.assign<ModelComponent>().lock()
								->set_casts_shadow(true)
								.set_casts_reflection(false)
								.set_model(model);

							es->drop();
							es->select(object);
						}
					}
				}
			}
		}
		
	}

	bool is_parent_of(TransformComponent* trans, TransformComponent* selectedTransform)
	{
		if (!selectedTransform)
			return false;

		if (trans == selectedTransform)
		{
			return true;
		}
		auto parent = selectedTransform->get_parent().lock().get();
		return is_parent_of(trans, parent);
	};

	void draw_entity(runtime::Entity entity)
	{
		if (!entity)
			return;

		gui::PushID(entity.id().index());
		gui::AlignFirstTextHeightToWidgets();
		auto es = core::get_subsystem<editor::EditState>();
		auto& selected = es->selection_data.object;

		bool isSselected = false;
		if (selected && selected.is_type<runtime::Entity>())
		{
			isSselected = selected.get_value<runtime::Entity>() == entity;
		}	
		
		bool isParentOfSelected = false;
// 		if (selected && selected.is_type<runtime::Entity>())
// 		{
// 			auto selectedEntity = selected.get_value<runtime::Entity>();
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
			noChildren = transformComponent->get_children().empty();
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

		check_context_menu(entity);
		check_drag(entity, hovered);
		if (opened)
		{
			if (!noChildren)
			{
				auto children = entity.component<TransformComponent>().lock()->get_children();
				for (auto& child : children)
				{
					if (!child.expired())
						draw_entity(child.lock()->get_entity());
				}
			}

			gui::TreePop();
		}

		gui::PopID();
	}

	void render_hierarchy(ImVec2 area)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		auto ts = core::get_subsystem<runtime::TransformSystem>();
		auto input = core::get_subsystem<runtime::Input>();

		auto& roots = ts->get_roots();

		auto& editorCamera = es->camera;
		auto& selected = es->selection_data.object;

		check_context_menu(runtime::Entity());

		if (gui::IsWindowFocused())
		{
			if (input->is_key_pressed(sf::Keyboard::Delete))
			{
				if (selected && selected.is_type<runtime::Entity>())
				{
					auto sel = selected.get_value<runtime::Entity>();
					if (sel != editorCamera)
					{
						sel.destroy();
						es->unselect();
					}
				}
			}

			if (input->is_key_pressed(sf::Keyboard::D))
			{
				if (input->is_key_down(sf::Keyboard::LControl))
				{
					if (selected && selected.is_type<runtime::Entity>())
					{
						auto sel = selected.get_value<runtime::Entity>();
						if (sel != editorCamera)
						{
							auto clone = ecs->create_from_copy(sel);
							clone.component<TransformComponent>().lock()
								->set_parent(sel.component<TransformComponent>().lock()->get_parent(), false, true);
							es->select(clone);
						}
					}
				}
			}
		}

		for (auto& root : roots)
		{
			if (!root.expired())
				draw_entity(root.lock()->get_entity());
		}		
	}

};