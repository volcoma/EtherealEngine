#include "hierarchy_dock.h"
#include "../../assets/asset_extensions.h"
#include "../../editing/editing_system.h"
#include "../../system/project_manager.h"
#include "core/filesystem/filesystem.h"
#include "core/logging/logging.h"
#include "core/system/subsystem.h"
#include "runtime/assets/asset_handle.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/constructs/prefab.h"
#include "runtime/ecs/constructs/utils.h"
#include "runtime/ecs/systems/scene_graph.h"
#include "runtime/input/input.h"
#include "runtime/rendering/mesh.h"

namespace
{
enum class context_action
{
	none,
	rename,
};
}

static context_action check_context_menu(runtime::entity entity)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& editor_camera = es.camera;
	context_action action = context_action::none;
	if(entity && entity != editor_camera)
	{
		if(gui::BeginPopupContextItem("Entity Context Menu"))
		{
			if(gui::MenuItem("CREATE CHILD"))
			{
				auto object = ecs.create();
				object.assign<transform_component>().lock()->set_parent(entity);
			}

			if(gui::MenuItem("RENAME", "F2"))
			{
				action = context_action::rename;
			}

			if(gui::MenuItem("CLONE", "CTRL + D"))
			{
				auto object = ecs::utils::clone_entity(entity);

				auto obj_trans_comp = object.get_component<transform_component>().lock();
				auto ent_trans_comp = entity.get_component<transform_component>().lock();
				if(obj_trans_comp && ent_trans_comp)
				{
					obj_trans_comp->set_parent(ent_trans_comp->get_parent(), false, true);
				}

				es.select(object);
			}
			if(gui::MenuItem("DELETE", "DEL"))
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
			if(gui::MenuItem("CREATE EMPTY"))
			{
				auto object = ecs.create();
				object.assign<transform_component>();
			}

			gui::EndPopup();
		}
	}
	return action;
}

static bool process_drag_drop_source(runtime::entity entity)
{
	if(entity && gui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		auto entity_index = entity.id().index();
		gui::TextUnformatted(entity.get_name().c_str());
		gui::SetDragDropPayload("entity", &entity_index, sizeof(entity_index));
		gui::EndDragDropSource();
		return true;
	}

	return false;
}

static void process_drag_drop_target(runtime::entity entity)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& es = core::get_subsystem<editor::editing_system>();

	if(gui::BeginDragDropTarget())
	{
		if(gui::IsDragDropPayloadBeingAccepted())
		{
			gui::SetMouseCursor(ImGuiMouseCursor_Move);
		}
		{
			auto payload = gui::AcceptDragDropPayload("entity");
			if(payload)
			{
				std::uint32_t entity_index = 0;
				std::memcpy(&entity_index, payload->Data, std::size_t(payload->DataSize));
				if(ecs.valid_index(entity_index))
				{
					auto eid = ecs.create_id(entity_index);
					auto dropped_entity = ecs.get(eid);
					if(dropped_entity)
					{
						auto trans_comp = dropped_entity.get_component<transform_component>().lock();
						if(trans_comp)
						{
							trans_comp->set_parent(entity);
						}
					}
				}
			}
		}

		for(const auto& type : ex::get_suported_formats<prefab>())
		{
			auto payload = gui::AcceptDragDropPayload(type.c_str());
			if(payload)
			{
				std::string absolute_path(reinterpret_cast<const char*>(payload->Data),
										  std::size_t(payload->DataSize));

				std::string key = fs::convert_to_protocol(fs::path(absolute_path)).string();
				using asset_t = prefab;
				using entry_t = asset_handle<asset_t>;
				auto entry = entry_t{};
				auto entry_future = am.find_asset_entry<asset_t>(key);
				if(entry_future.is_ready())
				{
					entry = entry_future.get();
				}
				if(entry)
				{
					auto object = entry->instantiate();
					auto trans_comp = object.get_component<transform_component>().lock();
					if(trans_comp)
					{
						trans_comp->set_parent(entity);
					}
					es.select(object);
				}
			}
		}
		for(const auto& type : ex::get_suported_formats<mesh>())
		{
			auto payload = gui::AcceptDragDropPayload(type.c_str());
			if(payload)
			{
				std::string absolute_path(reinterpret_cast<const char*>(payload->Data),
										  std::size_t(payload->DataSize));

				std::string key = fs::convert_to_protocol(fs::path(absolute_path)).string();
				using asset_t = mesh;
				using entry_t = asset_handle<asset_t>;
				auto entry = entry_t{};
				auto entry_future = am.find_asset_entry<asset_t>(key);
				if(entry_future.is_ready())
				{
					entry = entry_future.get();
				}
				if(entry)
				{
					model mdl;
					mdl.set_lod(entry, 0);

					auto object = ecs.create();
					// Add component and configure it.
					object.assign<transform_component>().lock()->set_parent(entity);
					// Add component and configure it.
					object.assign<model_component>()
						.lock()
						->set_casts_shadow(true)
						.set_casts_reflection(false)
						.set_model(mdl);

					es.select(object);
				}
			}
		}

		gui::EndDragDropTarget();
	}
}

static void check_drag(runtime::entity entity)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& editor_camera = es.camera;

	if(entity != editor_camera)
	{
		if(!process_drag_drop_source(entity))
		{
			process_drag_drop_target(entity);
		}
	}
}

void hierarchy_dock::draw_entity(runtime::entity entity)
{
	if(!entity)
		return;

	gui::PushID(static_cast<int>(entity.id().index()));
	gui::PushID(static_cast<int>(entity.id().version()));

	gui::AlignTextToFramePadding();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& selected = es.selection_data.object;
	bool is_selected = false;
	if(selected && selected.is_type<runtime::entity>())
	{
		is_selected = selected.get_value<runtime::entity>() == entity;
	}

	std::string name = entity.to_string();
	ImGuiTreeNodeFlags flags = 0 | ImGuiTreeNodeFlags_AllowOverlapMode | ImGuiTreeNodeFlags_OpenOnArrow;

	if(is_selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if(gui::IsWindowFocused())
	{
		if(is_selected && !gui::IsAnyItemActive())
		{
			if(input.is_key_pressed(mml::keyboard::F2))
			{
				edit_label_ = true;
				gui::SetKeyboardFocusHere();
			}
		}
	}
	auto transformComponent = entity.get_component<transform_component>().lock();
	bool no_children = true;
	if(transformComponent)
		no_children = transformComponent->get_children().empty();

	if(no_children)
		flags |= ImGuiTreeNodeFlags_Leaf;

	auto pos = gui::GetCursorScreenPos();
	gui::AlignTextToFramePadding();

	bool opened = gui::TreeNodeEx(name.c_str(), flags);
	if(!edit_label_)
	{
		check_drag(entity);
	}
	if(edit_label_ && is_selected)
	{
		std::array<char, 64> input_buff;
		input_buff.fill(0);
		std::memcpy(input_buff.data(), name.c_str(),
					name.size() < input_buff.size() ? name.size() : input_buff.size());

		gui::SetCursorScreenPos(pos);
		gui::PushItemWidth(gui::GetContentRegionAvailWidth());

		gui::PushID(static_cast<int>(entity.id().index()));
		gui::PushID(static_cast<int>(entity.id().version()));
		if(gui::InputText("", input_buff.data(), input_buff.size(),
						  ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			entity.set_name(input_buff.data());
			edit_label_ = false;
		}

		gui::PopItemWidth();

		if(!gui::IsItemActive() && (gui::IsMouseClicked(0) || gui::IsMouseDragging()))
		{
			edit_label_ = false;
		}
		gui::PopID();
		gui::PopID();
	}

	ImGuiWindow* window = gui::GetCurrentWindow();

	if(gui::IsItemHovered() && !gui::IsMouseDragging(0))
	{
		if(gui::IsMouseClicked(0))
		{
			id_ = window->GetID(transformComponent.get());
		}

		if(gui::IsMouseReleased(0) && window->GetID(transformComponent.get()) == id_)
		{
			if(!is_selected)
				edit_label_ = false;

			es.select(entity);
		}

		if(gui::IsMouseDoubleClicked(0))
		{
			edit_label_ = is_selected;
		}
	}

	if(!edit_label_)
	{
		auto action = check_context_menu(entity);
		if(action == context_action::rename)
		{
			edit_label_ = true;
		}
	}

	if(opened)
	{
		if(!no_children)
		{
			const auto& children = entity.get_component<transform_component>().lock()->get_children();
			for(auto& child : children)
			{
				if(child.valid())
					draw_entity(child);
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
	auto& sg = core::get_subsystem<runtime::scene_graph>();
	auto& input = core::get_subsystem<runtime::input>();

	auto& roots = sg.get_roots();
	auto& editor_camera = es.camera;
	auto& selected = es.selection_data.object;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

	if(gui::BeginChild("hierarchy_content", gui::GetContentRegionAvail(), false, flags))
	{

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
							auto clone = ecs::utils::clone_entity(sel);
							auto clone_trans_comp = clone.get_component<transform_component>().lock();
							auto sel_trans_comp = sel.get_component<transform_component>().lock();
							if(clone_trans_comp && sel_trans_comp)
							{
								clone_trans_comp->set_parent(sel_trans_comp->get_parent(), false, true);
							}
							es.select(clone);
						}
					}
				}
			}
		}

		if(editor_camera.valid())
		{
			draw_entity(editor_camera);
			gui::Separator();
		}

		for(auto& root : roots)
		{
			if(root.valid())
			{
				if(root != editor_camera)
					draw_entity(root);
			}
		}
	}
	gui::EndChild();
	process_drag_drop_target({});
}

hierarchy_dock::hierarchy_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{

	initialize(dtitle, close_button, min_size,
			   std::bind(&hierarchy_dock::render, this, std::placeholders::_1));
}
