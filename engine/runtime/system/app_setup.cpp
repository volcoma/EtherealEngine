#include "app_setup.h"
#include "../assets/asset_manager.h"
#include "../assets/impl/asset_reader.h"
#include "../assets/impl/asset_writer.h"

#include "../animation/animation.h"
#include "../ecs/constructs/prefab.h"
#include "../ecs/constructs/scene.h"
#include "../rendering/material.h"
#include "../rendering/mesh.h"
#include "core/audio/sound.h"
#include "core/graphics/shader.h"
#include "core/graphics/texture.h"

namespace runtime
{

void setup_asset_manager()
{
    auto& manager = core::get_subsystem<asset_manager>();
	{
		auto& storage = manager.add_storage<gfx::shader>();
		storage.load_from_file = asset_reader::load_from_file<gfx::shader>;
		storage.load_from_instance = asset_reader::load_from_instance<gfx::shader>;
	}
	{
		auto& storage = manager.add_storage<gfx::texture>();
		storage.load_from_file = asset_reader::load_from_file<gfx::texture>;
		storage.load_from_instance = asset_reader::load_from_instance<gfx::texture>;
	}
	{
		auto& storage = manager.add_storage<mesh>();
		storage.load_from_file = asset_reader::load_from_file<mesh>;
		storage.load_from_instance = asset_reader::load_from_instance<mesh>;
	}
	{
		auto& storage = manager.add_storage<audio::sound>();
		storage.load_from_file = asset_reader::load_from_file<audio::sound>;
		storage.load_from_instance = asset_reader::load_from_instance<audio::sound>;
	}
	{
		auto& storage = manager.add_storage<material>();
		storage.load_from_file = asset_reader::load_from_file<material>;
		storage.load_from_instance = asset_reader::load_from_instance<material>;
	}
	{
		auto& storage = manager.add_storage<animation>();
		storage.load_from_file = asset_reader::load_from_file<animation>;
		storage.load_from_instance = asset_reader::load_from_instance<animation>;
	}
	{
		auto& storage = manager.add_storage<prefab>();
		storage.load_from_file = asset_reader::load_from_file<prefab>;
		storage.load_from_instance = asset_reader::load_from_instance<prefab>;
	}
	{
		auto& storage = manager.add_storage<scene>();
		storage.load_from_file = asset_reader::load_from_file<scene>;
		storage.load_from_instance = asset_reader::load_from_instance<scene>;
	}

	{
		const auto id = "embedded:/sphere";
		auto instance = std::make_shared<mesh>();
		instance->create_sphere(gfx::mesh_vertex::get_layout(), 0.5f, 20, 20, false,
								mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/cube";
		auto instance = std::make_shared<mesh>();
		instance->create_cube(gfx::mesh_vertex::get_layout(), 1.0f, 1.0f, 1.0f, 1, 1, 1, false,
							  mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/plane";
		auto instance = std::make_shared<mesh>();
		instance->create_cube(gfx::mesh_vertex::get_layout(), 10.0f, 0.01f, 10.0f, 1, 1, 1, false,
							  mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/cylinder";
		auto instance = std::make_shared<mesh>();
		instance->create_cylinder(gfx::mesh_vertex::get_layout(), 0.5f, 2.0f, 20, 20, false,
								  mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/capsule";
		auto instance = std::make_shared<mesh>();
		instance->create_capsule(gfx::mesh_vertex::get_layout(), 0.5f, 2.0f, 20, 20, false,
								 mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/cone";
		auto instance = std::make_shared<mesh>();
		instance->create_cone(gfx::mesh_vertex::get_layout(), 0.5f, 0.0f, 2, 20, 20, false,
							  mesh_create_origin::bottom);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/torus";
		auto instance = std::make_shared<mesh>();
		instance->create_torus(gfx::mesh_vertex::get_layout(), 1.0f, 0.5f, 20, 20, false,
							   mesh_create_origin::center);
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/teapot";
		auto instance = std::make_shared<mesh>();
		instance->create_teapot(gfx::mesh_vertex::get_layout());
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/tetrahedron";
		auto msh = std::make_shared<mesh>();
		msh->create_tetrahedron(gfx::mesh_vertex::get_layout());
		manager.load_asset_from_instance(id, msh);
	}
	{
		const auto id = "embedded:/octahedron";
		auto instance = std::make_shared<mesh>();
		instance->create_octahedron(gfx::mesh_vertex::get_layout());
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/icosahedron";
		auto instance = std::make_shared<mesh>();
		instance->create_icosahedron(gfx::mesh_vertex::get_layout());
		manager.load_asset_from_instance(id, instance);
	}
	{
		const auto id = "embedded:/dodecahedron";
		auto instance = std::make_shared<mesh>();
		instance->create_dodecahedron(gfx::mesh_vertex::get_layout());
		manager.load_asset_from_instance(id, instance);
	}

	for(int i = 0; i < 6; ++i)
	{
		const auto id = std::string("embedded:/icosphere") + std::to_string(i);
		auto instance = std::make_shared<mesh>();
		instance->create_icosphere(gfx::mesh_vertex::get_layout(), i);
		manager.load_asset_from_instance(id, instance);
	}

	{
		const auto id = "embedded:/standard";
		std::shared_ptr<material> instance = std::make_shared<standard_material>();
		manager.load_asset_from_instance(id, instance);
	}
}
}
