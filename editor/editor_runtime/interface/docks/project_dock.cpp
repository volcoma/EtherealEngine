#include "project_dock.h"
#include "../../assets/asset_extensions.h"
#include "../../editing/editing_system.h"
#include "core/audio/sound.h"
#include "core/common/nonstd/function_traits.hpp"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"
#include "core/string_utils/string_utils.h"
#include "core/system/subsystem.h"
#include "core/tasks/task_system.h"
#include "editor_core/nativefd/filedialog.h"
#include "runtime/animation/animation.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/impl/asset_writer.h"
#include "runtime/ecs/constructs/prefab.h"
#include "runtime/ecs/constructs/scene.h"
#include "runtime/ecs/constructs/utils.h"
#include "runtime/ecs/ecs.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include <chrono>

using namespace std::literals;

template <typename T>
static asset_handle<gfx::texture> get_preview(const T& entry, const std::string& type,
											  editor::editing_system& es)
{
	if(entry)
	{
		return es.icons[type];
	}
	return es.icons["loading"];
};

static asset_handle<gfx::texture> get_preview(const asset_handle<gfx::texture>& entry, const std::string&,
											  editor::editing_system& es)
{
	if(entry)
	{
		return entry;
	}
	return es.icons["loading"];
};

static asset_handle<gfx::texture> get_preview(const fs::path&, const std::string& type,
											  editor::editing_system& es)
{
	return es.icons[type];
};

static bool process_drag_drop_source(const asset_handle<gfx::texture>& preview, const fs::path& absolute_path)
{
	if(gui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		const auto filename = absolute_path.filename();
		const std::string extension = filename.has_extension() ? filename.extension().string() : "folder";
		const std::string id = absolute_path.string();
		const std::string strfilename = filename.string();
		const auto& tex = preview;
		ImVec2 item_size = {32, 32};
		ImVec2 texture_size = item_size;
		if(tex)
		{
			texture_size = {float(tex->info.width), float(tex->info.height)};
		}
		ImVec2 uv0 = {0.0f, 0.0f};
		ImVec2 uv1 = {1.0f, 1.0f};
		gui::BeginGroup();

		auto w = gui::CalcTextSize(strfilename.c_str()).x;

		gui::SameLine(0.0f, (w - item_size.x) / 2.0f);
		gui::ImageWithAspect(gui::get_info(tex), texture_size, item_size, uv0, uv1);

		gui::TextUnformatted(strfilename.c_str());

		gui::EndGroup();
		gui::SetDragDropPayload(extension.c_str(), id.data(), id.size());
		gui::EndDragDropSource();
		return true;
	}

	return false;
}

static void process_drag_drop_target(const fs::path& absolute_path)
{
	if(gui::BeginDragDropTarget())
	{
		if(gui::IsDragDropPayloadBeingAccepted())
		{
			gui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		else
		{
			gui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
		}

		fs::error_code err;
		if(fs::is_directory(absolute_path, err))
		{
			static const std::vector<std::vector<std::string>> types = ex::get_all_formats();

			const auto process_drop = [&absolute_path](const std::string& type) {
				auto payload = gui::AcceptDragDropPayload(type.c_str());
				if(payload != nullptr)
				{
					std::string data(reinterpret_cast<const char*>(payload->Data),
									 std::size_t(payload->DataSize));
					fs::path new_name = absolute_path / fs::path(data).filename();
					if(data != new_name)
					{
						fs::error_code err;

						if(!fs::exists(new_name, err))
						{
							fs::rename(data, new_name, err);
						}
					}
				}
				return payload;
			};

			for(const auto& asset_set : types)
			{
				for(const auto& type : asset_set)
				{
					if(process_drop(type) != nullptr)
					{
						break;
					}
				}
			}
			{
				process_drop("folder");
			}
			{
				auto payload = gui::AcceptDragDropPayload("entity");
				if(payload != nullptr)
				{
					std::uint32_t entity_index = 0;
					std::memcpy(&entity_index, payload->Data, std::size_t(payload->DataSize));
					auto& ecs = core::get_subsystem<runtime::entity_component_system>();
					if(ecs.valid_index(entity_index))
					{
						auto eid = ecs.create_id(entity_index);
						auto dropped_entity = ecs.get(eid);
						if(dropped_entity)
						{
							auto prefab_path = absolute_path /
											   fs::path(dropped_entity.to_string() + ".pfb").make_preferred();
							ecs::utils::save_entity_to_file(prefab_path, dropped_entity);
						}
					}
				}
			}
		}
		gui::EndDragDropTarget();
	}
}

static bool draw_entry(asset_handle<gfx::texture> icon, bool is_loading, const std::string& name,
					   const fs::path& absolute_path, bool is_selected, const float size,
					   const std::function<void()>& on_click, const std::function<void()>& on_double_click,
					   const std::function<void(const std::string&)>& on_rename,
					   const std::function<void()>& on_delete)
{
	enum class entry_action
	{
		none,
		clicked,
		double_clicked,
		renamed,
		deleted,
	};

	bool is_popup_opened = false;
	entry_action action = entry_action::none;

	bool open_rename_menu = false;

	gui::PushID(name.c_str());
	if(is_selected && !gui::IsAnyItemActive() && gui::IsWindowFocused())
	{
		if(gui::IsKeyPressed(mml::keyboard::F2))
		{
			open_rename_menu = true;
		}

		if(gui::IsKeyPressed(mml::keyboard::Delete))
		{
			action = entry_action::deleted;
		}
	}

	if(gui::GetContentRegionAvailWidth() < size)
	{
		gui::NewLine();
	}

	ImVec2 item_size = {size, size};
	ImVec2 texture_size = item_size;
	if(icon)
	{
		texture_size = {float(icon->info.width), float(icon->info.height)};
	}
	ImVec2 uv0 = {0.0f, 0.0f};
	ImVec2 uv1 = {1.0f, 1.0f};

	auto col = gui::GetStyle().Colors[ImGuiCol_WindowBg];
	gui::PushStyleColor(ImGuiCol_Button, ImVec4(col.x, col.y, col.z, 0.44f));
	gui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col.x, col.y, col.z, 0.86f));
	gui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(col.x, col.y, col.z, 1.0f));

	const int padding = 6;
	auto pos = gui::GetCursorScreenPos();
	pos.y += item_size.y + padding * 2.0f;
	if(gui::ImageButtonWithAspectAndTextDOWN(gui::get_info(icon), name, texture_size, item_size, uv0, uv1,
											 padding))
	{
		action = entry_action::clicked;
	}

	gui::PopStyleColor(3);

	if(gui::IsItemHovered())
	{
		if(on_double_click)
		{
			gui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		if(gui::IsMouseDoubleClicked(0))
		{
			action = entry_action::double_clicked;
		}

		auto& g = *gui::GetCurrentContext();
		if(!g.DragDropActive)
		{
			gui::BeginTooltip();
			gui::TextUnformatted(name.c_str());
			gui::EndTooltip();
		}
	}

	if(is_selected && gui::GetNavInputAmount(ImGuiNavInput_Input, ImGuiInputReadMode_Pressed) > 0.0f)
	{
		action = entry_action::double_clicked;
	}

	std::array<char, 64> input_buff;
	input_buff.fill(0);
	auto name_sz = std::min(name.size(), input_buff.size() - 1);
	std::memcpy(input_buff.data(), name.c_str(), name_sz);

	if(gui::BeginPopupContextItem("ENTRY_CONTEXT_MENU"))
	{
		is_popup_opened = true;

		if(gui::MenuItem("RENAME", "F2"))
		{
			open_rename_menu = true;
			gui::CloseCurrentPopup();
		}

		if(gui::MenuItem("DELETE", "DEL"))
		{
			action = entry_action::deleted;
			gui::CloseCurrentPopup();
		}
		gui::EndPopup();
	}

	if(open_rename_menu)
	{
		gui::OpenPopup("ENTRY_RENAME_MENU");
		gui::SetNextWindowPos(pos);
	}

	if(gui::BeginPopup("ENTRY_RENAME_MENU"))
	{
		is_popup_opened = true;
		if(open_rename_menu)
		{
			gui::SetKeyboardFocusHere();
		}
		gui::PushItemWidth(150.0f);
		if(gui::InputText("##NAME", input_buff.data(), input_buff.size(),
						  ImGuiInputTextFlags_EnterReturnsTrue))
		{
			action = entry_action::renamed;
			gui::CloseCurrentPopup();
		}
		gui::PopItemWidth();
		gui::EndPopup();
	}
	if(is_selected)
	{
		gui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
	}

	if(is_loading)
	{
		action = entry_action::none;
	}

	switch(action)
	{
		case entry_action::clicked:
		{
			if(on_click)
			{
				on_click();
			}
		}
		break;
		case entry_action::double_clicked:
		{
			if(on_double_click)
			{
				on_double_click();
			}
		}
		break;
		case entry_action::renamed:
		{
			std::string new_name = std::string(input_buff.data());
			if(new_name != name && !new_name.empty())
			{
				if(on_rename)
				{
					on_rename(new_name);
				}
			}
		}
		break;
		case entry_action::deleted:
		{
			if(on_delete)
			{
				on_delete();
			}
		}
		break;

		default:
			break;
	}

	if(!process_drag_drop_source(icon, absolute_path))
	{
		process_drag_drop_target(absolute_path);
	}

	gui::PopID();
	gui::SameLine();
	return is_popup_opened;
}

fs::path get_new_file(const fs::path& path, const std::string& name, const std::string& ext = "")
{
	int i = 0;
	fs::error_code err;
	while(fs::exists(path / (string_utils::format("%s (%d)", name.c_str(), i) + ext), err))
	{
		++i;
	}

	return path / (string_utils::format("%s (%d)", name.c_str(), i) + ext);
}

void project_dock::render(const ImVec2& /*unused*/)
{
	const auto root_path = fs::resolve_protocol("app:/data");

	fs::error_code err;
	if(root_ != root_path || !fs::exists(cache_.get_path(), err))
	{
		root_ = root_path;
		cache_.set_path(root_);
	}

	if(gui::Button("IMPORT..."))
	{
		import();
	}
	gui::SameLine();
	gui::PushItemWidth(80.0f);
	gui::SliderFloat("", &scale_, 0.3f, 1.0f);
	const float size = 128.0f * scale_;
	if(gui::IsItemHovered())
	{
		gui::BeginTooltip();
		gui::TextUnformatted("SCALE ICONS");
		gui::EndTooltip();
	}
	gui::PopItemWidth();

	const auto hierarchy = fs::split_until(cache_.get_path(), root_path);

	int i = 0;
	for(const auto& dir : hierarchy)
	{
		gui::SameLine();
		gui::AlignTextToFramePadding();
		gui::TextUnformatted("/");
		gui::SameLine();

		gui::PushID(i++);
		bool clicked = gui::Button(dir.filename().string().c_str());
		gui::PopID();

		if(clicked)
		{
			set_cache_path(dir);
			break;
		}
		process_drag_drop_target(dir);
	}

	gui::Separator();

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
	fs::path current_path = cache_.get_path();
	if(gui::BeginChild("assets_content", gui::GetContentRegionAvail(), false, flags))
	{
		const auto is_selected = [&](const auto& entry) {
			using entry_t = std::decay_t<decltype(entry)>;
			auto& selected = es.selection_data.object;

			bool is_selected = selected.is_type<entry_t>() ? (selected.get_value<entry_t>() == entry) : false;
			return is_selected;
		};

		bool is_popup_opened = false;
		for(const auto& cache_entry : cache_)
		{
			const auto& absolute_path = cache_entry.path();
			auto filename = absolute_path.filename();
			auto name = filename.string();

			while(filename.has_extension())
			{
				filename = filename.stem();
				name = filename.string();
			}

			const auto relative = fs::convert_to_protocol(absolute_path).generic_string();
			const auto file_ext = absolute_path.extension().string();

			const auto on_rename = [&](const std::string& new_name) {
				fs::path new_absolute_path = absolute_path;
				new_absolute_path.remove_filename();
				new_absolute_path /= new_name + file_ext;
				fs::error_code err;
				fs::rename(absolute_path, new_absolute_path, err);
			};

			const auto on_delete = [&]() {
				fs::error_code err;
				fs::remove(absolute_path, err);

				es.unselect();
			};

			if(fs::is_directory(cache_entry.status()))
			{

				using entry_t = fs::path;
				entry_t entry = absolute_path;
				auto icon = get_preview(entry, "folder", es);
				is_popup_opened |= draw_entry(icon, false, name, absolute_path, is_selected(entry), size,
											  [&]() // on_click
											  {
												  es.select(entry);

											  },
											  [&]() // on_double_click
											  {
												  current_path = entry;
												  es.try_unselect<fs::path>();
											  },
											  [&](const std::string& new_name) // on_rename
											  {
												  fs::path new_absolute_path = absolute_path;
												  new_absolute_path.remove_filename();
												  new_absolute_path /= new_name;
												  fs::error_code err;
												  fs::rename(absolute_path, new_absolute_path, err);
											  },
											  [&]() // on_delete
											  {
												  fs::error_code err;
												  fs::remove_all(absolute_path, err);

											  });
			}
			else
			{

				bool processed = false;
				for(const auto& ext : ex::get_suported_formats<gfx::texture>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = gfx::texture;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "texture", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}

				for(const auto& ext : ex::get_suported_formats<mesh>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = mesh;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "mesh", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}

				for(const auto& ext : ex::get_suported_formats<audio::sound>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = audio::sound;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "sound", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}
				for(const auto& ext : ex::get_suported_formats<gfx::shader>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = gfx::shader;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "shader", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}
				for(const auto& ext : ex::get_suported_formats<material>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = material;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "material", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}

				for(const auto& ext : ex::get_suported_formats<runtime::animation>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = runtime::animation;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "animation", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}

				for(const auto& ext : ex::get_suported_formats<prefab>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = prefab;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "prefab", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   nullptr, // on_double_click
									   on_rename, on_delete);
					}
				}
				for(const auto& ext : ex::get_suported_formats<scene>())
				{
					if(processed)
					{
						break;
					}

					if(file_ext == ext)
					{
						processed = true;
						using asset_t = scene;
						using entry_t = asset_handle<asset_t>;
						auto entry = entry_t{};
						auto entry_future = am.find_asset_entry<asset_t>(relative);
						if(entry_future.is_ready())
						{
							entry = entry_future.get();
						}
						auto icon = get_preview(entry, "scene", es);
						bool is_loading = !entry;
						is_popup_opened |=
							draw_entry(icon, is_loading, name, absolute_path, is_selected(entry), size,
									   [&]() // on_click
									   {
										   es.select(entry);

									   },
									   [&]() // on_double_click
									   {
										   if(!entry)
										   {
											   return;
										   }

										   entry->instantiate(scene::mode::standard);
										   es.scene = fs::resolve_protocol(entry.id()).string();
										   es.load_editor_camera();

									   },
									   on_rename, on_delete);
					}
				}
			}
		}

		if(!is_popup_opened)
		{
			context_menu();
		}
		set_cache_path(current_path);
	}
	gui::EndChild();

	process_drag_drop_target(current_path);
}

void project_dock::context_menu()
{

	if(gui::BeginPopupContextWindow())
	{
		context_create_menu();

		gui::Separator();

		if(gui::Selectable("OPEN IN ENVIRONMENT"))
		{
			fs::show_in_graphical_env(cache_.get_path());
		}

		gui::Separator();

		if(gui::Selectable("IMPORT..."))
		{
			import();
		}

		gui::EndPopup();
	}
}

void project_dock::context_create_menu()
{
	if(gui::BeginMenu("CREATE"))
	{
		if(gui::MenuItem("FOLDER"))
		{
			const auto available = get_new_file(cache_.get_path(), "New Folder");
			fs::error_code err;
			fs::create_directory(available, err);
		}

		gui::Separator();

		if(gui::MenuItem("MATERIAL"))
		{
			const auto available = get_new_file(cache_.get_path(), "New Material", ".mat");
			const auto key = fs::convert_to_protocol(available).generic_string();

			auto& am = core::get_subsystem<runtime::asset_manager>();
			auto new_mat_future =
				am.load_asset_from_instance<material>(key, std::make_shared<standard_material>());
			auto asset = new_mat_future.get();
			runtime::asset_writer::save_to_file(asset.id(), asset);
		}

		gui::EndMenu();
	}
}

void project_dock::set_cache_path(const fs::path& path)
{
	if(cache_.get_path() == path)
	{
		return;
	}
	cache_.set_path(path);
	cache_path_with_protocol_ = fs::convert_to_protocol(path).generic();
}

void project_dock::import()
{
	std::vector<std::string> paths;
	if(native::open_multiple_files_dialog("", "", paths))
	{
		auto& ts = core::get_subsystem<core::task_system>();

		for(auto& path : paths)
		{
			fs::path p = fs::path(path).make_preferred();
			fs::path filename = p.filename();

			auto task = ts.push_on_worker_thread(
				[opened = cache_.get_path()](const fs::path& path, const fs::path& filename) {
					fs::error_code err;
					fs::path dir = opened / filename;
					fs::copy_file(path, dir, fs::copy_options::overwrite_if_exists, err);
				},
				p, filename);
		}
	}
}

project_dock::project_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{
	initialize(dtitle, close_button, min_size, std::bind(&project_dock::render, this, std::placeholders::_1));
}
