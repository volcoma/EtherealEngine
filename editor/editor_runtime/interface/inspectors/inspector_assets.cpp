#include "inspector_assets.h"
#include "../../editing/editing_system.h"
#include "core/audio/sound.h"
#include "core/filesystem/filesystem.h"
#include "core/graphics/texture.h"
#include "core/system/subsystem.h"
#include "inspectors.h"
#include "runtime/animation/animation.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/constructs/prefab.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"

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
		available = gui::GetContentRegionAvailWidth();

	ImVec2 size = {available, available};
	if(data)
	{
		auto asset_sz = data.link->asset->get_size();
		float w = float(asset_sz.width);
		float h = float(asset_sz.height);

		gui::ImageWithAspect(data.link->asset, ImVec2(w, h), size);
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
	}
	gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
	bool hoveredFrame = gui::IsItemHoveredRect();
	auto bbMinFrame = gui::GetItemRectMin();
	auto bbMaxFrame = gui::GetItemRectMax();

	if(selected && !selected.is_type<asset_handle<gfx::texture>>())
	{
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
		if((!gui::IsItemActive() && gui::IsItemHovered()) || hoveredFrame)
		{
			// gui::SetMouseCursor(ImGuiMouseCursor_Help);
			gui::BeginTooltip();
			gui::TextUnformatted("YOU CAN DRAG AND DROP HERE");
			gui::EndTooltip();
		}

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<gfx::texture>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 1.0f);
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(hoveredFrame || gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 2.0f);
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();

				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<gfx::texture>>();
					var = data;
					changed = true;
				}
			}
		}
		return changed;
	}

	if(data)
	{
		auto info = data.get()->info;
		rttr::variant vari = info;
		changed |= inspect_var(vari);
	}
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

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<material>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<material>>();
					var = data;
					return true;
				}
			}
		}
		return false;
	}

	if(gui::Button("SAVE CHANGES##top", ImVec2(-1, 0)))
	{
		am.save(data);
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
		am.save(data);
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

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<mesh>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<mesh>>();
					var = data;
					return true;
				}
			}
		}
		return false;
	}

	bool changed = false;

	if(data)
	{
		mesh::info info;
		info.vertices = data->get_vertex_count();
		info.primitives = data->get_face_count();
		info.subsets = static_cast<std::uint32_t>(data->get_subset_count());
		rttr::variant vari = info;
		changed |= inspect_var(vari);
	}
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

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<runtime::animation>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<runtime::animation>>();
					var = data;
					return true;
				}
			}
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

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<audio::sound>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<audio::sound>>();
					var = data;
					return true;
				}
			}
		}
		return false;
	}

	bool changed = false;

	if(data)
	{
		rttr::variant vari = data.get()->get_data();
		changed |= inspect_var(vari);
	}
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

		auto& dragged = es.drag_data.object;
		if(dragged && dragged.is_type<asset_handle<prefab>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if(gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if(gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<prefab>>();
					var = data;
					return true;
				}
			}
		}
		return false;
	}

	return false;
}
