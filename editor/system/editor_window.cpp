#include "editor_window.h"
#include "../editing/editing_system.h"
#include "core/filesystem/filesystem.h"
#include "core/logging/logging.h"
#include "filedialog/filedialog.h"
#include "project_manager.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/reflection_probe_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/systems/scene_graph.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/system/engine.h"

std::vector<runtime::entity> gather_scene_data()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& sg = core::get_subsystem<runtime::scene_graph>();
	const auto& roots = sg.get_roots();
	auto editor_camera = es.camera;
	std::vector<runtime::entity> entities;
	for(auto root : roots)
	{
		auto entity = root.lock()->get_entity();
		if(entity != editor_camera)
			entities.push_back(entity);
	}

	return entities;
}

void default_scene()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();

	{
		auto object = ecs.create();
		object.set_name("main camera");
		object.assign<transform_component>().lock()->set_local_position({0.0f, 2.0f, -5.0f});
		object.assign<camera_component>();
	}
	{
		auto object = ecs.create();
		object.set_name("light");
		object.assign<transform_component>()
			.lock()
			->set_local_position({1.0f, 6.0f, -3.0f})
			.rotate_local(50.0f, -30.0f, 0.0f);

		light light_data;
		light_data.color = math::color(255, 244, 214, 255);
		object.assign<light_component>().lock()->set_light(light_data);
	}
	{
		auto object = ecs.create();
		object.set_name("global probe");
		object.assign<transform_component>().lock()->set_local_position({0.0f, 0.1f, 0.0f});

		reflection_probe probe;
		probe.method = reflect_method::environment;
		probe.type = probe_type::sphere;
		probe.sphere_data.range = 1000.0f;
		object.assign<reflection_probe_component>().lock()->set_probe(probe);
	}
	{
		auto object = ecs.create();
		object.set_name("local probe");
		object.assign<transform_component>().lock()->set_local_position({0.0f, 0.1f, 0.0f});

		reflection_probe probe;
		probe.method = reflect_method::static_only;
		probe.type = probe_type::box;
		object.assign<reflection_probe_component>().lock()->set_probe(probe);
	}
	{
		auto object = ecs.create();
		object.set_name("platform");
		object.assign<transform_component>();

		auto asset_future = am.load<mesh>("embedded:/plane");
		model model;
		model.set_lod(asset_future.get(), 0);

		// Add component and configure it.
		object.assign<model_component>().lock()->set_casts_shadow(true).set_casts_reflection(true).set_model(
			model);
	}
	{
		auto object = ecs.create();
		object.set_name("object");
		object.assign<transform_component>().lock()->set_local_position({0.0f, 0.5f, 0.0f});

		auto asset_future = am.load<mesh>("embedded:/sphere");
		model model;
		model.set_lod(asset_future.get(), 0);

		// Add component and configure it.
		object.assign<model_component>().lock()->set_casts_shadow(true).set_casts_reflection(false).set_model(
			model);
	}
}

auto create_new_scene()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	es.save_editor_camera();
	ecs.dispose();
	es.load_editor_camera();
	default_scene();
	es.scene.clear();
}

auto open_scene()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	std::string path;
	if(open_file_dialog(extensions::scene.substr(1), fs::resolve_protocol("app:/data").string(), path))
	{
		es.save_editor_camera();
		ecs.dispose();
		es.load_editor_camera();

		std::vector<runtime::entity> outData;
		if(ecs::utils::load_data(path, outData))
		{
			es.scene = path;
		}
	}
}

auto save_scene()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	const auto& path = es.scene;
	if(path != "")
	{
		std::vector<runtime::entity> entities = gather_scene_data();
		ecs::utils::save_data(path, entities);
	}

	es.save_editor_camera();
}

void save_scene_as()
{
	auto& es = core::get_subsystem<editor::editing_system>();

	std::string path;
	if(save_file_dialog(extensions::scene.substr(1), fs::resolve_protocol("app:/data").string(), path))
	{
		es.scene = path;
		if(!fs::path(path).has_extension())
			es.scene += extensions::scene;

		save_scene();
	}

	es.save_editor_camera();
}

main_editor_window::main_editor_window()
{
}

main_editor_window::main_editor_window(mml::video_mode mode, const std::string& title,
									   std::uint32_t style /*= mml::style::Default*/)
	: gui_window(mode, title, style)
{
}

main_editor_window::~main_editor_window()
{
}

void main_editor_window::on_gui(std::chrono::duration<float> dt)
{
	if(_show_start_page)
	{
		gui::PushFont(gui::GetFont("editor_big"));
		on_start_page();
		gui::PopFont();
	}
	else
	{
		on_menubar();
		on_toolbar();
	}

	gui_window::on_gui(dt);
}

void main_editor_window::on_menubar()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& pm = core::get_subsystem<editor::project_manager>();
	auto& input = core::get_subsystem<runtime::input>();
	const auto& current_project = pm.get_current_project();

	if(input.is_key_down(mml::keyboard::LControl))
	{
		if(input.is_key_down(mml::keyboard::LShift))
		{
			if(input.is_key_pressed(mml::keyboard::S))
			{
				save_scene_as();
			}
		}
		else if(input.is_key_pressed(mml::keyboard::S))
		{
			save_scene();
		}

		if(input.is_key_pressed(mml::keyboard::O))
		{
			open_scene();
		}

		if(input.is_key_pressed(mml::keyboard::N))
		{
			create_new_scene();
		}
	}
	if(gui::BeginMainMenuBar())
	{

		if(gui::BeginMenu("FILE"))
		{
			if(gui::MenuItem("NEW SCENE", "CTRL+N", false, current_project != ""))
			{
				create_new_scene();
			}
			if(gui::MenuItem("OPEN SCENE", "CTRL+O", false, current_project != ""))
			{
				open_scene();
			}
			if(gui::MenuItem("SHOW START PAGE", "CTRL+P"))
			{
				_show_start_page = true;
				pm.close_project();
				restore();
				auto& io = gui::GetIO();
				io.MouseDown[0] = false;
				io.MouseDown[1] = false;
				io.MouseDown[2] = false;
			}

			if(gui::MenuItem("SAVE", "CTRL+S", false, es.scene != "" && current_project != ""))
			{
				save_scene();
			}
			auto& ecs = core::get_subsystem<runtime::entity_component_system>();

			if(gui::MenuItem("SAVE AS..", "CTRL+SHIFT+S", false, ecs.size() > 0 && current_project != ""))
			{
				save_scene_as();
			}

			gui::EndMenu();
		}
		if(gui::BeginMenu("EDIT"))
		{
			if(gui::MenuItem("UNDO", "CTRL+Z"))
			{
			}
			if(gui::MenuItem("REDO", "CTRL+Y", false, false))
			{
			}
			gui::Separator();
			if(gui::MenuItem("CUT", "CTRL+X"))
			{
			}
			if(gui::MenuItem("COPY", "CTRL+C"))
			{
			}
			if(gui::MenuItem("PASTE", "CTRL+V"))
			{
			}
			gui::EndMenu();
		}
		if(gui::BeginMenu("WINDOWS"))
		{
			gui::EndMenu();
		}
		float offset = gui::GetWindowHeight();
		gui::EndMainMenuBar();
		gui::SetCursorPosY(gui::GetCursorPosY() + offset);
	}
}

void main_editor_window::on_toolbar()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& icons = es.icons;

	float width = gui::GetContentRegionAvailWidth();
	if(gui::ToolbarButton(icons["translate"].get(), "TRANSLATE",
						  es.operation == imguizmo::operation::translate))
	{
		es.operation = imguizmo::operation::translate;
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["rotate"].get(), "ROTATE", es.operation == imguizmo::operation::rotate))
	{
		es.operation = imguizmo::operation::rotate;
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["scale"].get(), "SCALE", es.operation == imguizmo::operation::scale))
	{
		es.operation = imguizmo::operation::scale;
		es.mode = imguizmo::mode::local;
	}
	gui::SameLine(0.0f, 50.0f);

	if(gui::ToolbarButton(icons["local"].get(), "LOCAL COORDINATE SYSTEM", es.mode == imguizmo::mode::local))
	{
		es.mode = imguizmo::mode::local;
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["global"].get(), "GLOBAL COORDINATE SYSTEM", es.mode == imguizmo::mode::world,
						  es.operation != imguizmo::operation::scale))
	{
		es.mode = imguizmo::mode::world;
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["grid"].get(), "SHOW GRID", es.show_grid))
	{
		es.show_grid = !es.show_grid;
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["wireframe"].get(), "WIREFRAME SELECTION", es.wireframe_selection))
	{
		es.wireframe_selection = !es.wireframe_selection;
	}

	gui::SameLine(width / 2.0f - 36.0f);
	if(gui::ToolbarButton(icons["play"].get(), "PLAY", false))
	{
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["pause"].get(), "PAUSE", false))
	{
	}
	gui::SameLine(0.0f);
	if(gui::ToolbarButton(icons["next"].get(), "STEP", false))
	{
	}
}

void main_editor_window::render_dockspace()
{
	if(!_show_start_page)
	{
		_dockspace.update_and_draw(
			ImVec2(gui::GetContentRegionAvail().x,
				   gui::GetContentRegionAvail().y - gui::GetItemsLineHeightWithSpacing()));

		auto items = _console_log->get_items();
		if(!items.empty())
		{
			auto& last_item = items.back();
			const auto& colorization = _console_log->get_level_colorization(last_item.second);
			ImVec4 col = {colorization[0], colorization[1], colorization[2], colorization[3]};

			ImGui::SetCursorPosY(ImGui::GetCursorPosY());
			gui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::AlignFirstTextHeightToWidgets();
			if(gui::Selectable(last_item.first.c_str(), false, 0, ImVec2(0, gui::GetTextLineHeight())))
			{
				_dockspace.activate_dock(_console_dock_name);
			}
			gui::PopStyleColor();
		}
	}
}

void main_editor_window::on_start_page()
{
	auto& pm = core::get_subsystem<editor::project_manager>();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar |
							 ImGuiWindowFlags_NoSavedSettings;

	gui::AlignFirstTextHeightToWidgets();
	gui::TextUnformatted("RECENT PROJECTS");
	gui::Separator();
	gui::BeginGroup();
	{
		if(gui::BeginChild("projects_content",
						   ImVec2(gui::GetContentRegionAvail().x * 0.7f, gui::GetContentRegionAvail().y),
						   false, flags))
		{

			const auto& rencent_projects = pm.get_options().recent_project_paths;
			for(const auto& path : rencent_projects)
			{
				if(gui::Selectable(path.c_str()))
				{
					pm.open_project(path);
					maximize();
					_show_start_page = false;
				}
			}
		}
		gui::EndChild();
	}
	gui::EndGroup();

	gui::SameLine();

	gui::BeginGroup();
	{
		if(gui::Button("NEW PROJECT", ImVec2(gui::GetContentRegionAvailWidth(), 0.0f)))
		{
			std::string path;
			if(pick_folder_dialog("", path))
			{
				pm.create_project(path);
				maximize();
				_show_start_page = false;
			}
		}

		if(gui::Button("OPEN OTHER", ImVec2(gui::GetContentRegionAvailWidth(), 0.0f)))
		{
			std::string path;
			if(pick_folder_dialog("", path))
			{
				pm.open_project(path);
				maximize();
				_show_start_page = false;
			}
		}
	}
	gui::EndGroup();
}

void main_editor_window::show_start_page(bool show)
{
	_show_start_page = show;
}

void main_editor_window::set_log(const std::string& name, std::shared_ptr<console_log> log)
{
	_console_dock_name = name;
	_console_log = log;
}
