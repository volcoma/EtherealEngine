#include "hierarchy_dock.h"
#include "../../edit_state.h"
#include "../../project.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/systems/scene_graph.h"
#include "runtime/input/input.h"
#include "runtime/system/filesystem.h"
#include "runtime/rendering/mesh.h"
#include "runtime/assets/asset_handle.h"

void check_context_menu(runtime::Entity entity)
{
	auto es = core::get_subsystem<editor::EditState>();
	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
	auto& editor_camera = es->camera;
	if (entity && entity != editor_camera)
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

void check_drag(runtime::Entity entity)
{
	if (!gui::IsWindowHovered())
		return;

	auto es = core::get_subsystem<editor::EditState>();
	auto& editor_camera = es->camera;
	auto& dragged = es->drag_data.object;

	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

	if (entity)
	{
		if (gui::IsItemHoveredRect())
		{


			if (gui::IsMouseClicked(gui::drag_button) &&
				entity != editor_camera)
			{
				es->drag(entity, entity.to_string());
			}

			if (dragged && entity != editor_camera)
			{
				if (dragged.is_type<runtime::Entity>())
				{
					auto dragged_entity = dragged.get_value<runtime::Entity>();
					if (dragged_entity && dragged_entity != entity)
					{
						gui::SetMouseCursor(ImGuiMouseCursor_Move);
						if (gui::IsMouseReleased(gui::drag_button))
						{
							dragged_entity.component<TransformComponent>().lock()
								->set_parent(entity.component<TransformComponent>());

							es->drop();
						}
					}
				}

				if (dragged.is_type<AssetHandle<Prefab>>())
				{
					gui::SetMouseCursor(ImGuiMouseCursor_Move);
					if (gui::IsMouseReleased(gui::drag_button))
					{
						auto prefab = dragged.get_value<AssetHandle<Prefab>>();
						auto object = prefab->instantiate();
						object.component<TransformComponent>().lock()
							->set_parent(entity.component<TransformComponent>());

						es->drop();
						es->select(object);
					}
				}
				if (dragged.is_type<AssetHandle<Mesh>>())
				{
					gui::SetMouseCursor(ImGuiMouseCursor_Move);
					if (gui::IsMouseReleased(gui::drag_button))
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
	}

}

void draw_entity(runtime::Entity entity)
{
	if (!entity)
		return;

	gui::PushID(entity.id().index());
	gui::AlignFirstTextHeightToWidgets();
	auto es = core::get_subsystem<editor::EditState>();
	auto input = core::get_subsystem<runtime::Input>();
	auto& selected = es->selection_data.object;
	static bool edit_label = false;
	bool is_selected = false;
	if (selected && selected.is_type<runtime::Entity>())
	{
		is_selected = selected.get_value<runtime::Entity>() == entity;
	}

	std::string name = entity.to_string();
	ImGuiTreeNodeFlags flags = 0
		| ImGuiTreeNodeFlags_AllowOverlapMode
		| ImGuiTreeNodeFlags_OpenOnArrow;

	if (is_selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (is_selected && !gui::IsAnyItemActive())
	{
		if (input->is_key_pressed(sf::Keyboard::F2))
		{
			edit_label = true;
		}
	}

	auto transformComponent = entity.component<TransformComponent>().lock();
	bool no_children = true;
	if (transformComponent)
		no_children = transformComponent->get_children().empty();

	if (no_children)
		flags |= ImGuiTreeNodeFlags_Leaf;



	auto pos = gui::GetCursorScreenPos();
	gui::AlignFirstTextHeightToWidgets();
	bool opened = gui::TreeNodeEx(name.c_str(), flags);

	if (edit_label && is_selected)
	{
		static std::string inputBuff(64, 0);
		std::memset(&inputBuff[0], 0, 64);
		std::memcpy(&inputBuff[0], name.c_str(), name.size() < 64 ? name.size() : 64);

		gui::SetCursorScreenPos(pos);
		gui::PushID(transformComponent.get());
		gui::PushItemWidth(gui::GetContentRegionAvailWidth());
		if (gui::InputText("",
			&inputBuff[0],
			inputBuff.size(),
			ImGuiInputTextFlags_EnterReturnsTrue))
		{
			entity.set_name(inputBuff.c_str());
			edit_label = false;
		}
		gui::PopItemWidth();

		if (!gui::IsItemActive() && (gui::IsMouseClicked(0) || gui::IsMouseDragging()))
		{
			edit_label = false;
		}
		gui::PopID();
	}

	ImGuiWindow* window = gui::GetCurrentWindow();
	static ImGuiID id;

	if (gui::IsItemHovered() && !gui::IsMouseDragging(0))
	{
		if (gui::IsMouseClicked(0))
		{
			id = window->GetID(transformComponent.get());
		}

		if (gui::IsMouseReleased(0) && window->GetID(transformComponent.get()) == id)
		{
			if (!is_selected)
				edit_label = false;

			es->select(entity);
		}

		if (gui::IsMouseDoubleClicked(0))
		{
			edit_label = is_selected;
		}
	}

	if (!edit_label)
	{
		check_context_menu(entity);
		check_drag(entity);
	}


	if (opened)
	{
		if (!no_children)
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

void HierarchyDock::render(const ImVec2& area)
{
	auto es = core::get_subsystem<editor::EditState>();
	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
	auto sg = core::get_subsystem<runtime::SceneGraph>();
	auto input = core::get_subsystem<runtime::Input>();

	auto& roots = sg->get_roots();

	auto& editor_camera = es->camera;
	auto& selected = es->selection_data.object;
	auto& dragged = es->drag_data.object;

	check_context_menu(runtime::Entity());

	if (gui::IsWindowFocused())
	{
		if (input->is_key_pressed(sf::Keyboard::Delete))
		{
			if (selected && selected.is_type<runtime::Entity>())
			{
				auto sel = selected.get_value<runtime::Entity>();
				if (sel && sel != editor_camera)
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
					if (sel && sel != editor_camera)
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
		{
			auto entity = root.lock()->get_entity();
			draw_entity(entity);
			if (entity == editor_camera)
				gui::Separator();
		}


	}

	if (gui::IsWindowHovered() && !gui::IsAnyItemHovered())
	{

		if (dragged)
		{
			if (dragged.is_type<runtime::Entity>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if (gui::IsMouseReleased(gui::drag_button))
				{
					auto dragged_entity = dragged.get_value<runtime::Entity>();
					if (dragged_entity)
					{
						dragged_entity.component<TransformComponent>().lock()
							->set_parent(runtime::CHandle<TransformComponent>());
					}

					es->drop();
				}
			}
			if (dragged.is_type<AssetHandle<Prefab>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if (gui::IsMouseReleased(gui::drag_button))
				{
					auto prefab = dragged.get_value<AssetHandle<Prefab>>();
					auto object = prefab->instantiate();
					es->drop();
					es->select(object);
				}
			}
			if (dragged.is_type<AssetHandle<Mesh>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if (gui::IsMouseReleased(gui::drag_button))
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

HierarchyDock::HierarchyDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{

	initialize(dtitle, dcloseButton, dminSize, std::bind(&HierarchyDock::render, this, std::placeholders::_1));
}

