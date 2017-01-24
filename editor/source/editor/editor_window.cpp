#include "editor_window.h"
#include "edit_state.h"
#include "project.h"

#include "filedialog/filedialog.h"
#include "runtime/ecs/systems/transform_system.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/ecs/utils.h"
#include "runtime/system/filesystem.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/input/input.h"
#include "core/logging/logging.h"

std::vector<runtime::Entity> gather_scene_data()
{
	auto es = core::get_subsystem<editor::EditState>();
	auto ts = core::get_subsystem<runtime::TransformSystem>();
	const auto& roots = ts->get_roots();
	auto editor_camera = es->camera;
	std::vector<runtime::Entity> entities;
	for (auto root : roots)
	{
		auto entity = root.lock()->get_entity();
		if (entity != editor_camera)
			entities.push_back(entity);
	}

	return entities;
}


void default_scene()
{
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

	{
		auto object = ecs->create();
		object.set_name("main camera");
		object.assign<TransformComponent>().lock()
			->set_local_position({ 0.0f, 2.0f, -5.0f });
		object.assign<CameraComponent>();
	}
	{
		auto object = ecs->create();
		object.set_name("light");
		object.assign<TransformComponent>().lock()
			->set_local_position({ 1.0f, 6.0f, -3.0f })
			.rotate_local(50.0f, -30.0f, 0.0f);
		object.assign<LightComponent>();
	}
	{
		auto object = ecs->create();
		object.set_name("platform");
		object.assign<TransformComponent>().lock()
			->set_local_scale({ 10.0f, 1.0f, 10.0f });

		Model model;
		am->load<Mesh>("engine_data:/meshes/platform", false)
			.then([&model](auto asset)
		{
			model.set_lod(asset, 0);
		});

		//Add component and configure it.
		object.assign<ModelComponent>().lock()
			->set_casts_shadow(true)
			.set_casts_reflection(false)
			.set_model(model);
	}
	{
		auto object = ecs->create();
		object.set_name("object");
		object.assign<TransformComponent>().lock()
			->set_local_position({ 0.0f, 0.1f, 0.0f });

		Model model;
		am->load<Mesh>("engine_data:/meshes/bunny", false)
			.then([&model](auto asset)
		{
			model.set_lod(asset, 0);
		});

		//Add component and configure it.
		object.assign<ModelComponent>().lock()
			->set_casts_shadow(true)
			.set_casts_reflection(false)
			.set_model(model);
	}
}

void save_editor_camera()
{
	auto es = core::get_subsystem<editor::EditState>();
	if (es->camera)
		ecs::utils::save_data(fs::resolve_protocol("app:/settings/editor_camera.cfg"), { es->camera });
}

void load_editor_camera()
{
	auto es = core::get_subsystem<editor::EditState>();
	runtime::Entity object;
	if (!ecs::utils::try_load_entity(fs::resolve_protocol("app:/settings/editor_camera.cfg"), object))
	{
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		object = ecs->create();
		object.set_name("EDITOR CAMERA");
		object.assign<TransformComponent>().lock()
			->set_local_position({ 0.0f, 2.0f, -5.0f });
		object.assign<CameraComponent>();
	}
	
	es->camera = object;
}


auto create_new_scene()
{
	auto es = core::get_subsystem<editor::EditState>();
	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
	save_editor_camera();
	ecs->dispose();
	load_editor_camera();
	default_scene();
	es->scene.clear();
}

auto open_scene()
{
	auto es = core::get_subsystem<editor::EditState>();
	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
	std::string path;
	if (open_file_dialog("sgr", fs::resolve_protocol("app:/data").string(), path))
	{
		save_editor_camera();
		ecs->dispose();
		load_editor_camera();

		std::vector<runtime::Entity> outData;
		if (ecs::utils::load_data(path, outData))
		{
			es->scene = path;
		}
	}
}

auto save_scene()
{
	auto es = core::get_subsystem<editor::EditState>();
	const auto& path = es->scene;
	if (path != "")
	{
		std::vector<runtime::Entity> entities = gather_scene_data();
		ecs::utils::save_data(path, entities);
	}

	save_editor_camera();
}

void save_scene_as()
{
	auto es = core::get_subsystem<editor::EditState>();

	std::string path;
	if (save_file_dialog("sgr", fs::resolve_protocol("app:/data").string(), path))
	{
		es->scene = path + ".sgr";		
		save_scene();	
	}

	save_editor_camera();
}


MainEditorWindow::MainEditorWindow()
{
	maximize();
}

MainEditorWindow::MainEditorWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
	:GuiWindow(mode, title, style)
{
	maximize();
}

MainEditorWindow::~MainEditorWindow()
{
}

void MainEditorWindow::on_gui(std::chrono::duration<float> dt)
{
	if (gui::BeginMainMenuBar())
	{
		on_menubar();

		float offset = gui::GetWindowHeight();
		gui::EndMainMenuBar();
		gui::SetCursorPosY(gui::GetCursorPosY() + offset);
	}

	
	on_toolbar();
	GuiWindow::on_gui(dt);
}

void MainEditorWindow::on_menubar()
{
	auto es = core::get_subsystem<editor::EditState>();
	auto pm = core::get_subsystem<editor::ProjectManager>();
	auto input = core::get_subsystem<runtime::Input>();
	const auto& current_project = pm->get_current_project();

	if (input->is_key_down(sf::Keyboard::LControl))
	{
		if (input->is_key_down(sf::Keyboard::LShift))
		{
			if (input->is_key_pressed(sf::Keyboard::S))
			{
				save_scene_as();
			}
		}
		else if (input->is_key_pressed(sf::Keyboard::S))
		{
			save_scene();
		}

		if (input->is_key_pressed(sf::Keyboard::O))
		{
			open_scene();
		}

		if (input->is_key_pressed(sf::Keyboard::N))
		{
			create_new_scene();
		}
	}

	if (gui::BeginMenu("File"))
	{
		if (gui::MenuItem("New Scene", "Ctrl+N", false, current_project != ""))
		{
			create_new_scene();
		}
		if (gui::MenuItem("Open Scene", "Ctrl+O", false, current_project != ""))
		{
			open_scene();
		}
		if (gui::MenuItem("Open Project Manager", "Ctrl+P"))
		{
			pm->open();
		}

		if (gui::MenuItem("Save", "Ctrl+S", false, es->scene != "" && current_project != ""))
		{
			save_scene();
		}
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

		if (gui::MenuItem("Save As..", "Ctrl+Shift+S", false, ecs->size() > 0 && current_project != ""))
		{
			save_scene_as();
		}

		gui::EndMenu();
	}
	if (gui::BeginMenu("Edit"))
	{
		if (gui::MenuItem("Undo", "CTRL+Z"))
		{

		}
		if (gui::MenuItem("Redo", "CTRL+Y", false, false))
		{

		}
		gui::Separator();
		if (gui::MenuItem("Cut", "CTRL+X"))
		{

		}
		if (gui::MenuItem("Copy", "CTRL+C"))
		{

		}
		if (gui::MenuItem("Paste", "CTRL+V"))
		{

		}
		gui::EndMenu();
	}

}

void MainEditorWindow::on_toolbar()
{
	auto es = core::get_subsystem<editor::EditState>();
	auto& icons = es->icons;

	float width = gui::GetContentRegionAvailWidth();
	if (gui::ToolbarButton(icons["translate"].get(), "Translate", es->operation == imguizmo::OPERATION::TRANSLATE))
	{
		es->operation = imguizmo::OPERATION::TRANSLATE;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["rotate"].get(), "Rotate", es->operation == imguizmo::OPERATION::ROTATE))
	{
		es->operation = imguizmo::OPERATION::ROTATE;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["scale"].get(), "Scale", es->operation == imguizmo::OPERATION::SCALE))
	{
		es->operation = imguizmo::OPERATION::SCALE;
		es->mode = imguizmo::MODE::LOCAL;
	}
	gui::SameLine(0.0f, 50.0f);

	if (gui::ToolbarButton(icons["local"].get(), "Local Coordinate System", es->mode == imguizmo::MODE::LOCAL))
	{
		es->mode = imguizmo::MODE::LOCAL;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["global"].get(), "Global Coordinate System", es->mode == imguizmo::MODE::WORLD, es->operation != imguizmo::OPERATION::SCALE))
	{
		es->mode = imguizmo::MODE::WORLD;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["grid"].get(), "Show Grid", es->show_grid))
	{
		es->show_grid = !es->show_grid;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["wireframe"].get(), "Wireframe Selection", es->wireframe_selection))
	{
		es->wireframe_selection = !es->wireframe_selection;
	}

	gui::SameLine(width / 2.0f - 36.0f);
	if (gui::ToolbarButton(icons["play"].get(), "Play", false))
	{

	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["pause"].get(), "Pause", false))
	{

	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["next"].get(), "Step", false))
	{

	}
}

ProjectManagerWindow::ProjectManagerWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
	: GuiWindow(mode, title, style)
{

}

void ProjectManagerWindow::on_gui(std::chrono::duration<float> dt)
{
	auto es = core::get_subsystem<editor::EditState>();
	auto pm = core::get_subsystem<editor::ProjectManager>();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoSavedSettings;

	static bool recompile_assets = true;
	gui::Text("Recent Projects");
	gui::Separator();
	gui::BeginGroup();
	{
		if (gui::BeginChild("###projects_content", ImVec2(gui::GetContentRegionAvail().x * 0.8f, gui::GetContentRegionAvail().y - gui::GetTextLineHeightWithSpacing()), false, flags))
		{
			
			const auto& rencent_projects = pm->get_recent_projects();
			for (auto& path : rencent_projects)
			{
				if (gui::Selectable(path.c_str()))
				{
					pm->open_project(path, recompile_assets);
					load_editor_camera();
					set_main(false);
					close();
				}
			}
			gui::EndChild();
		}
		gui::Checkbox("Recompile Assets", &recompile_assets);

		if (gui::IsItemHoveredRect())
		{
			gui::SetTooltip(
			"Force to recompile all assets when a project is opened.\n"
			"This will create compiled versions of the raw ones \n"
			"which will be loaded into the app."
			);
		}
	}
	gui::EndGroup();

	gui::SameLine();
	
	gui::BeginGroup();
	{
		if (gui::Button("NEW PROJECT"))
		{
			std::string path;
			if (pick_folder_dialog("", path))
			{
				pm->create_project(path);
				load_editor_camera();
				set_main(false);
				close();
			}
		}

		if (gui::Button("OPEN OTHER"))
		{
			std::string path;
			if (pick_folder_dialog("", path))
			{
				pm->open_project(path, recompile_assets);
				load_editor_camera();
				set_main(false);
				close();
			}
		}

		
	}
	gui::EndGroup();
	

	GuiWindow::on_gui(dt);
}