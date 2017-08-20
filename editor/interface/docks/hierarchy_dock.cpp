#include "hierarchy_dock.h"
#include "../../editing/editing_system.h"
#include "../../system/project_manager.h"
#include "core/filesystem/filesystem.h"
#include "runtime/assets/asset_handle.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/systems/scene_graph.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "runtime/rendering/mesh.h"

void check_context_menu(runtime::entity entity)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& editor_camera = es.camera;
	if(entity && entity != editor_camera)
	{
		if(gui::BeginPopupContextItem("Entity Context Menu"))
		{
			if(gui::Selectable("CREATE CHILD"))
			{
				auto object = ecs.create();
				object.assign<transform_component>().lock()->set_parent(
					entity.get_component<transform_component>());
			}
			if(gui::Selectable("CLONE"))
			{
				auto object = ecs.create_from_copy(entity);
				object.get_component<transform_component>().lock()->set_parent(
					entity.get_component<transform_component>().lock()->get_parent(), false, true);

				es.select(object);
			}
			if(gui::Selectable("DELETE"))
			{
				entity.destroy();
			}

			gui::EndPopup();
		}
	}
	else
	{
		if(gui::BeginPopupContextWindow())
		{
			if(gui::Selectable("CREATE EMPTY"))
			{
				auto object = ecs.create();
				object.assign<transform_component>();
			}

			gui::EndPopup();
		}
	}
}

void check_drag(runtime::entity entity)
{
	if(!gui::IsWindowHovered())
		return;

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& editor_camera = es.camera;
	auto& dragged = es.drag_data.object;

	auto& ecs = core::get_subsystem<runtime::entity_component_system>();

	if(entity)
	{
		if(gui::IsItemHoveredRect())
		{

			if(gui::IsMouseClicked(gui::drag_button) && entity != editor_camera)
			{
				es.drag(entity, entity.to_string());
			}

			if(dragged && entity != editor_camera)
			{
				if(dragged.is_type<runtime::entity>())
				{
					auto dragged_entity = dragged.get_value<runtime::entity>();
					if(dragged_entity && dragged_entity != entity)
					{
						gui::SetMouseCursor(ImGuiMouseCursor_Move);
						if(gui::IsMouseReleased(gui::drag_button))
						{
							dragged_entity.get_component<transform_component>().lock()->set_parent(
								entity.get_component<transform_component>());

							es.drop();
						}
					}
				}

				if(dragged.is_type<asset_handle<prefab>>())
				{
					gui::SetMouseCursor(ImGuiMouseCursor_Move);
					if(gui::IsMouseReleased(gui::drag_button))
					{
						auto pfab = dragged.get_value<asset_handle<prefab>>();
						auto object = pfab->instantiate();
						object.get_component<transform_component>().lock()->set_parent(
							entity.get_component<transform_component>());

						es.drop();
						es.select(object);
					}
				}
				if(dragged.is_type<asset_handle<mesh>>())
				{
					gui::SetMouseCursor(ImGuiMouseCursor_Move);
					if(gui::IsMouseReleased(gui::drag_button))
					{
						auto hmesh = dragged.get_value<asset_handle<mesh>>();
						model mdl;
						mdl.set_lod(hmesh, 0);

						auto object = ecs.create();
						// Add component and configure it.
						object.assign<transform_component>().lock()->set_parent(
							entity.get_component<transform_component>());
						// Add component and configure it.
						object.assign<model_component>()
							.lock()
							->set_casts_shadow(true)
							.set_casts_reflection(false)
							.set_model(mdl);

						es.drop();
						es.select(object);
					}
				}
			}
		}
	}
}

void draw_entity(runtime::entity entity)
{
	if(!entity)
		return;

	gui::PushID(static_cast<int>(entity.id().index()));
    gui::PushID(static_cast<int>(entity.id().version()));
    
	gui::AlignFirstTextHeightToWidgets();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& selected = es.selection_data.object;
	static bool edit_label = false;
	bool is_selected = false;
	if(selected && selected.is_type<runtime::entity>())
	{
		is_selected = selected.get_value<runtime::entity>() == entity;
	}

	std::string name = entity.to_string();
	ImGuiTreeNodeFlags flags = 0 | ImGuiTreeNodeFlags_AllowOverlapMode | ImGuiTreeNodeFlags_OpenOnArrow;

	if(is_selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if(is_selected && !gui::IsAnyItemActive())
	{
		if(input.is_key_pressed(mml::keyboard::F2))
		{
			edit_label = true;
		}
	}

	auto transformComponent = entity.get_component<transform_component>().lock();
	bool no_children = true;
	if(transformComponent)
		no_children = transformComponent->get_children().empty();

	if(no_children)
		flags |= ImGuiTreeNodeFlags_Leaf;

	auto pos = gui::GetCursorScreenPos();
	gui::AlignFirstTextHeightToWidgets();
	bool opened = gui::TreeNodeEx(name.c_str(), flags);

	if(edit_label && is_selected)
	{
		static std::string inputBuff(64, 0);
		std::memset(&inputBuff[0], 0, 64);
		std::memcpy(&inputBuff[0], name.c_str(), name.size() < 64 ? name.size() : 64);

		gui::SetCursorScreenPos(pos);
		gui::PushID(transformComponent.get());
		gui::PushItemWidth(gui::GetContentRegionAvailWidth());
		if(gui::InputText("", &inputBuff[0], inputBuff.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			entity.set_name(inputBuff.c_str());
			edit_label = false;
		}
		gui::PopItemWidth();

		if(!gui::IsItemActive() && (gui::IsMouseClicked(0) || gui::IsMouseDragging()))
		{
			edit_label = false;
		}
		gui::PopID();
	}

	ImGuiWindow* window = gui::GetCurrentWindow();
	static ImGuiID id;

	if(gui::IsItemHovered() && !gui::IsMouseDragging(0))
	{
		if(gui::IsMouseClicked(0))
		{
			id = window->GetID(transformComponent.get());
		}

		if(gui::IsMouseReleased(0) && window->GetID(transformComponent.get()) == id)
		{
			if(!is_selected)
				edit_label = false;

			es.select(entity);
		}

		if(gui::IsMouseDoubleClicked(0))
		{
			edit_label = is_selected;
		}
	}

	if(!edit_label)
	{
		check_context_menu(entity);
		check_drag(entity);
	}

	if(opened)
	{
		if(!no_children)
		{
			auto children = entity.get_component<transform_component>().lock()->get_children();
			for(auto& child : children)
			{
				if(!child.expired())
					draw_entity(child.lock()->get_entity());
			}
		}

		gui::TreePop();
	}

    gui::PopID();
	gui::PopID();
}

void hierarchy_dock::render(const ImVec2&)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& sg = core::get_subsystem<runtime::scene_graph>();
	auto& input = core::get_subsystem<runtime::input>();

	auto& roots = sg.get_roots();
	auto& editor_camera = es.camera;
	auto& selected = es.selection_data.object;
	auto& dragged = es.drag_data.object;

	check_context_menu(runtime::entity());

	if(gui::IsWindowFocused())
	{
		if(input.is_key_pressed(mml::keyboard::Delete))
		{
			if(selected && selected.is_type<runtime::entity>())
			{
				auto sel = selected.get_value<runtime::entity>();
				if(sel && sel != editor_camera)
				{
					sel.destroy();
					es.unselect();
				}
			}
		}

		if(input.is_key_pressed(mml::keyboard::D))
		{
			if(input.is_key_down(mml::keyboard::LControl))
			{
				if(selected && selected.is_type<runtime::entity>())
				{
					auto sel = selected.get_value<runtime::entity>();
					if(sel && sel != editor_camera)
					{
						auto clone = ecs.create_from_copy(sel);
						clone.get_component<transform_component>().lock()->set_parent(
							sel.get_component<transform_component>().lock()->get_parent(), false, true);
						es.select(clone);
					}
				}
			}
		}
	}

	for(auto& root : roots)
	{
		if(!root.expired())
		{
			auto entity = root.lock()->get_entity();
			draw_entity(entity);
			if(entity == editor_camera)
				gui::Separator();
		}
	}

	if(gui::IsWindowHovered() && !gui::IsAnyItemHovered())
	{

		if(dragged)
		{
			if(dragged.is_type<runtime::entity>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto dragged_entity = dragged.get_value<runtime::entity>();
					if(dragged_entity)
					{
						dragged_entity.get_component<transform_component>().lock()->set_parent(
							runtime::chandle<transform_component>());
					}

					es.drop();
				}
			}
			if(dragged.is_type<asset_handle<prefab>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto pfab = dragged.get_value<asset_handle<prefab>>();
					auto object = pfab->instantiate();
					es.drop();
					es.select(object);
				}
			}
			if(dragged.is_type<asset_handle<mesh>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto hmesh = dragged.get_value<asset_handle<mesh>>();
					model mdl;
					mdl.set_lod(hmesh, 0);

					auto object = ecs.create();
					// Add component and configure it.
					object.assign<transform_component>();
					// Add component and configure it.
					object.assign<model_component>()
						.lock()
						->set_casts_shadow(true)
						.set_casts_reflection(false)
						.set_model(mdl);

					es.drop();
					es.select(object);
				}
			}
		}
	}
}

hierarchy_dock::hierarchy_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{

	initialize(dtitle, dcloseButton, dminSize,
			   std::bind(&hierarchy_dock::render, this, std::placeholders::_1));
}
