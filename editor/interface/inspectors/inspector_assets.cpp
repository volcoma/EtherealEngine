#include "inspector_assets.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include "runtime/ecs/prefab.h"
#include "runtime/assets/asset_manager.h"
#include "core/filesystem/filesystem.h"
#include "inspectors.h"
#include "../../editing/editing_system.h"


bool inspector_asset_handle_texture::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<asset_handle<texture>>();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	bool is_selected = selected && selected.is_type<asset_handle<texture>>();
	bool changed = false;
	float available = math::min(64.0f, gui::GetContentRegionAvailWidth() / 1.5f);
	if (is_selected)
		available = gui::GetContentRegionAvailWidth();

	ImVec2 size = { available, available };
	if (data)
	{
		float w = float(data.link->asset->get_size().width);
		float h = float(data.link->asset->get_size().height);
		
		gui::ImageWithAspect(data.link->asset, ImVec2(w, h), size);
	}
	else
	{
		
		ImGuiWindow* window = gui::GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
		gui::ItemSize(bb);
		if (!gui::ItemAdd(bb, nullptr))
			return false;

	}
	gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
	bool hoveredFrame = gui::IsItemHoveredRect();
	auto bbMinFrame = gui::GetItemRectMin();
	auto bbMaxFrame = gui::GetItemBoxMax();
	
	if (selected && !selected.is_type<asset_handle<texture>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = asset_handle<texture>();
			}
			else
			{
				auto load_future = am.load<texture>(item);
				data = load_future.get();
			}
			var = data;
			return true;
		}

		auto& dragged = es.drag_data.object;
		if (dragged && dragged.is_type<asset_handle<texture>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 1.0f);
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (hoveredFrame || gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 2.0f);
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();

				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<texture>>();
					var = data;
					return true;

				}
			}
		}
		return false;
	}
	

	if(data)
	{
		rttr::variant vari = data.get()->info;
		changed |= inspect_var(vari);
	}
	return changed;

}


bool inspector_asset_handle_material::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<asset_handle<material>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if (selected && !selected.is_type<asset_handle<material>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = asset_handle<material>();
			}
			else
			{
				auto load_future = am.load<material>(item);
				data = load_future.get();
			}
			var = data;
			return true;
		}

		auto& dragged = es.drag_data.object;
		if (dragged && dragged.is_type<asset_handle<material>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<asset_handle<material>>();
					var = data;
					return true;

				}
			}
		}
		return false;
	}

	if (gui::Button("Apply to Asset"))
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
	if (gui::Button("Apply to Asset"))
	{
		am.save(data);
	}
	return changed;
}

bool inspector_asset_handle_mesh::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<asset_handle<mesh>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if (selected && !selected.is_type<asset_handle<mesh>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = asset_handle<mesh>();
			}
			else
			{
				auto load_future = am.load<mesh>(item);
				data = load_future.get();
			}
			var = data;
			return true;
		}
		
		auto& dragged = es.drag_data.object;
		if (dragged && dragged.is_type<asset_handle<mesh>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
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

	if (data)
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

bool inspector_asset_handle_prefab::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<asset_handle<prefab>>();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& selected = es.selection_data.object;
	if (selected && !selected.is_type<asset_handle<prefab>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = asset_handle<prefab>();
			}
			else
			{
				auto load_future = am.load<prefab>(item);
				data = load_future.get();

			}
			var = data;
			return true;
		}

		auto& dragged = es.drag_data.object;
		if (dragged && dragged.is_type<asset_handle<prefab>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
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
