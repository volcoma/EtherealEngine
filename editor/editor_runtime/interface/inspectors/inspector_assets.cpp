#include "inspector_assets.h"
#include "../../assets/asset_extensions.h"
#include "../../editing/editing_system.h"
#include "core/audio/sound.h"
#include "core/filesystem/filesystem.h"
#include "core/graphics/texture.h"
#include "core/system/subsystem.h"
#include "inspectors.h"
#include "runtime/animation/animation.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/impl/asset_writer.h"
#include "runtime/ecs/constructs/prefab.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"

template <typename asset_t>
static bool process_drag_drop_target(asset_handle<asset_t>& entry)
{
	auto& am = core::get_subsystem<runtime::asset_manager>();

	if(gui::BeginDragDropTarget())
	{
		if(gui::IsDragDropPayloadBeingAccepted())
		{
			gui::SetMouseCursor(ImGuiMouseCursor_Move);
		}
		for(const auto& type : ex::get_suported_formats<asset_t>())
		{
			auto payload = gui::AcceptDragDropPayload(type.c_str());
			if(payload)
			{
				std::string absolute_path(reinterpret_cast<const char*>(payload->Data),
										  std::size_t(payload->DataSize));

				std::string key = fs::convert_to_protocol(fs::path(absolute_path)).string();
				auto entry_future = am.find_asset_entry<asset_t>(key);
				if(entry_future.is_ready())
				{
					entry = entry_future.get();
				}

				if(entry)
				{
					return true;
				}
			}
		}
		gui::EndDragDropTarget();
	}
	return false;
	;
}

bool inspector_asset_handle_texture::inspect(rttr::variant& var, bool read_only,
											 const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<gfx::texture>>();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	bool is_selected = selected && selected.is_type<asset_handle<gfx::texture>>();
	bool changed = false;

	float available = math::min(64.0f, gui::GetContentRegionAvailWidth() / 1.5f);

	if(is_selected)
	{
		available = gui::GetContentRegionAvailWidth();
	}

	auto draw_image = [&]() {
		ImVec2 size = {available, available};
		if(data)
		{
			auto asset_sz = data.link->asset->get_size();
			float w = float(asset_sz.width);
			float h = float(asset_sz.height);
			const auto tex = data.link->asset;
			bool is_rt = tex ? tex->is_render_target() : false;
			bool is_orig_bl = gfx::is_origin_bottom_left();
			gui::ImageWithAspect(tex, is_rt, is_orig_bl, ImVec2(w, h), size);
		}
		else
		{

			ImGuiWindow* window = gui::GetCurrentWindow();
			if(window->SkipItems)
				return false;
			ImRect bb(window->DC.CursorPos,
					  ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
			gui::ItemSize(bb);
			if(!gui::ItemAdd(bb, 0))
				return false;

			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
		}
		return true;
	};

	if(selected && !selected.is_type<asset_handle<gfx::texture>>())
	{

		if(!draw_image())
		{
			return false;
		}

		if(process_drag_drop_target(data))
		{
			var = data;
			changed = true;
		}

		gui::SameLine();
		if(gui::Button("REMOVE"))
		{
			data = asset_handle<gfx::texture>();
			var = data;
			changed = true;
		}
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<gfx::texture>();
			}
			else
			{
				auto load_future = am.load<gfx::texture>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			changed = true;
		}

		if(process_drag_drop_target(data))
		{
			var = data;
			changed = true;
		}

		return changed;
	}

	gui::BeginTabBar("asset_handle_texture", ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoReorder |
												 ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);

	if(gui::TabItem("Info"))
	{
		if(!draw_image())
		{
			return false;
		}
		if(data)
		{
			auto info = data.get()->info;
			rttr::variant vari = info;
			changed |= inspect_var(vari);
		}
	}
	if(gui::TabItem("Import"))
	{
		gui::TextUnformatted("Import options");
	}
	gui::EndTabBar();

	return changed;
}

bool inspector_asset_handle_material::inspect(rttr::variant& var, bool read_only,
											  const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<material>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if(selected && !selected.is_type<asset_handle<material>>())
	{
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<material>();
			}
			else
			{
				auto load_future = am.load<material>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			return true;
		}

		if(process_drag_drop_target(data))
		{
			var = data;
			return true;
		}

		return false;
	}

	if(gui::Button("SAVE CHANGES##top", ImVec2(-1, 0)))
	{
		runtime::asset_writer::save_to_file(data.id(), data);
	}
	gui::Separator();
	bool changed = false;
	{
		rttr::variant vari = data.get();
		changed |= inspect_var(vari);
	}
	gui::Separator();
	if(gui::Button("SAVE CHANGES##bottom", ImVec2(-1, 0)))
	{
		runtime::asset_writer::save_to_file(data.id(), data);
	}
	return changed;
}

bool inspector_asset_handle_mesh::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<mesh>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if(selected && !selected.is_type<asset_handle<mesh>>())
	{
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<mesh>();
			}
			else
			{
				auto load_future = am.load<mesh>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			return true;
		}
		if(process_drag_drop_target(data))
		{
			var = data;
			return true;
		}
		return false;
	}

	bool changed = false;

	gui::BeginTabBar("asset_handle_mesh", ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoReorder |
											  ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);

	if(gui::TabItem("Info"))
	{
		if(data)
		{
			mesh::info info;
			info.vertices = data->get_vertex_count();
			info.primitives = data->get_face_count();
			info.subsets = static_cast<std::uint32_t>(data->get_subset_count());
			rttr::variant vari = info;
			changed |= inspect_var(vari);
		}
	}
	if(gui::TabItem("Import"))
	{
		gui::TextUnformatted("Import options");
	}
	gui::EndTabBar();

	return changed;
}

bool inspector_asset_handle_animation::inspect(rttr::variant& var, bool read_only,
											   const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<runtime::animation>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if(selected && !selected.is_type<asset_handle<runtime::animation>>())
	{
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<runtime::animation>();
			}
			else
			{
				auto load_future = am.load<runtime::animation>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			return true;
		}
		if(process_drag_drop_target(data))
		{
			var = data;
			return true;
		}
		return false;
	}

	bool changed = false;

	if(data)
	{
		rttr::variant vari = data.get();
		changed |= inspect_var(vari);
	}

	return changed;
}

bool inspector_asset_handle_sound::inspect(rttr::variant& var, bool read_only,
										   const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<audio::sound>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if(selected && !selected.is_type<asset_handle<audio::sound>>())
	{
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<audio::sound>();
			}
			else
			{
				auto load_future = am.load<audio::sound>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			return true;
		}

		if(process_drag_drop_target(data))
		{
			var = data;
			return true;
		}
		return false;
	}

	bool changed = false;

	gui::BeginTabBar("asset_handle_sound", ImGuiTabBarFlags_SizingPolicyEqual | ImGuiTabBarFlags_NoReorder |
											   ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);

	if(gui::TabItem("Info"))
	{
		if(data)
		{
			rttr::variant vari = data.get()->get_info();
			changed |= inspect_var(vari);
		}
	}
	if(gui::TabItem("Import"))
	{
		gui::TextUnformatted("Import options");
	}
	gui::EndTabBar();

	return changed;
}

bool inspector_asset_handle_prefab::inspect(rttr::variant& var, bool read_only,
											const meta_getter& get_metadata)
{
	auto data = var.get_value<asset_handle<prefab>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if(selected && !selected.is_type<asset_handle<prefab>>())
	{
		std::string item = !data.id().empty() ? data.id() : "none";
		rttr::variant var_str = item;
		if(inspect_var(var_str))
		{
			item = var_str.to_string();
			if(item.empty())
			{
				data = asset_handle<prefab>();
			}
			else
			{
				auto load_future = am.load<prefab>(item);
				if(load_future.valid())
				{
					data = load_future.get();
				}
			}
			var = data;
			return true;
		}

		if(process_drag_drop_target(data))
		{
			var = data;
			return true;
		}
		return false;
	}

	return false;
}
