#include "inspector_assets.h"
#include "inspectors.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/system/filesystem.h"
#include "core/serialization/archives.h"
#include "../../edit_state.h"
bool Inspector_AssetHandle_Texture::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<AssetHandle<Texture>>();
	auto es = core::get_subsystem<editor::EditState>();
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto& selected = es->selection_data.object;

	bool changed = false;
	float available = math::min(64.0f, gui::GetContentRegionAvailWidth() / 1.5f);
	ImVec2 size = { available, available };
	if (data)
	{
		gui::Image(data.link->asset, size);
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
	
	if (selected && !selected.is_type<AssetHandle<Texture>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = AssetHandle<Texture>();
			}
			else
			{
				am->load<Texture>(item, false)
					.then([&data](auto asset) mutable
				{
					data = asset;
				});
			}
			var = data;
			return true;
		}

		auto& dragged = es->drag_data.object;
		if (dragged && dragged.is_type<AssetHandle<Texture>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 2.0f);
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (hoveredFrame || gui::IsItemHoveredRect())
			{
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(bbMinFrame, bbMaxFrame, true, 0.0f, 2.0f);
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();

				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<AssetHandle<Texture>>();
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


bool Inspector_AssetHandle_Material::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<AssetHandle<Material>>();

	auto es = core::get_subsystem<editor::EditState>();
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto& selected = es->selection_data.object;
	if (selected && !selected.is_type<AssetHandle<Material>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = AssetHandle<Material>();
			}
			else
			{
				am->load<Material>(item, false)
					.then([&data](auto asset) mutable
				{
					data = asset;
				});
			}
			var = data;
			return true;
		}

		auto& dragged = es->drag_data.object;
		if (dragged && dragged.is_type<AssetHandle<Material>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<AssetHandle<Material>>();
					var = data;
					return true;

				}
			}
		}
		return false;
	}


	
	bool changed = false;
	{
		rttr::variant vari = data.get();
		changed |= inspect_var(vari);
	}
	gui::Separator();
	if (gui::Button("Apply to Asset"))
	{
		am->save(data);
	}
	return changed;
}

bool Inspector_AssetHandle_Mesh::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<AssetHandle<Mesh>>();

	auto es = core::get_subsystem<editor::EditState>();
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto& selected = es->selection_data.object;
	if (selected && !selected.is_type<AssetHandle<Mesh>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = AssetHandle<Mesh>();
			}
			else
			{
				am->load<Mesh>(item, false)
					.then([&data](auto asset) mutable
				{
					data = asset;
				});
			}
			var = data;
			return true;
		}
		
		auto& dragged = es->drag_data.object;
		if (dragged && dragged.is_type<AssetHandle<Mesh>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<AssetHandle<Mesh>>();
					var = data;
					return true;

				}
			}
		}
		return false;
	}

	bool changed = false;

	{
		rttr::variant vari = data.get()->info;
		changed |= inspect_var(vari);
	}
	return changed;
}

bool Inspector_AssetHandle_Prefab::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<AssetHandle<Prefab>>();

	auto es = core::get_subsystem<editor::EditState>();
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto& selected = es->selection_data.object;
	if (selected && !selected.is_type<AssetHandle<Prefab>>())
	{
		std::string item = data ? data.id() : "none";
		rttr::variant var_str = item;
		if (inspect_var(var_str))
		{
			item = var_str.to_string();
			if (item.empty())
			{
				data = AssetHandle<Prefab>();
			}
			else
			{
				am->load<Prefab>(item, false)
					.then([&data](auto asset) mutable
				{
					data = asset;
				});
			}
			var = data;
			return true;
		}

		auto& dragged = es->drag_data.object;
		if (dragged && dragged.is_type<AssetHandle<Prefab>>())
		{
			gui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.5f, 0.0f, 0.9f));
			gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 1.0f);
			gui::PopStyleColor();

			if (gui::IsItemHoveredRect())
			{
				gui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				gui::PopStyleColor();
				if (gui::IsMouseReleased(gui::drag_button))
				{
					data = dragged.get_value<AssetHandle<Prefab>>();
					var = data;
					return true;

				}
			}
		}
		return false;
	}

	return false;
}