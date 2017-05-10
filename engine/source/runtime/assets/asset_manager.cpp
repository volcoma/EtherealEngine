#include "asset_manager.h"
#include "asset_reader.h"
#include "asset_writer.h"
#include "asset_extensions.h"
#include "graphics/graphics.h"

#include "../rendering/mesh.h"
#include "../rendering/material.h"

namespace runtime
{
	bool asset_manager::initialize()
	{
		{
			auto storage = add<shader>();
			storage->ext = extensions::shader;
			storage->load_from_file = asset_reader::load_shader_from_file;
			storage->load_from_memory = asset_reader::load_shader_from_memory;
		}
		{
			auto storage = add<texture>();
			storage->ext = extensions::texture;
			storage->load_from_file = asset_reader::load_texture_from_file;
		}
		{
			auto storage = add<mesh>();
			storage->ext = extensions::mesh;
			storage->load_from_file = asset_reader::load_mesh_from_file;
			{
				auto id = "embedded:/sphere";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_sphere(gfx::mesh_vertex::decl, 0.5f, 20, 20, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/cube";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_cube(gfx::mesh_vertex::decl, 1.0f, 1.0f, 1.0f, 1, 1, 1, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/plane";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_cube(gfx::mesh_vertex::decl, 10.0f, 0.01f, 10.0f, 1, 1, 1, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/cylinder";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_cylinder(gfx::mesh_vertex::decl, 0.5f, 2.0f, 20, 20, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/capsule";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_capsule(gfx::mesh_vertex::decl, 0.5f, 2.0f, 20, 20, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/cone";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_cone(gfx::mesh_vertex::decl, 0.5f, 0.0f, 2, 20, 20, false, mesh_create_origin::bottom);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/torus";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_torus(gfx::mesh_vertex::decl, 1.0f, 0.5f, 20, 20, false, mesh_create_origin::center);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/teapot";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_teapot(gfx::mesh_vertex::decl);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/tetrahedron";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_tetrahedron(gfx::mesh_vertex::decl);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/octahedron";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_octahedron(gfx::mesh_vertex::decl);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/icosahedron";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_icosahedron(gfx::mesh_vertex::decl);
				request.set_data(id, msh);
			}
			{
				auto id = "embedded:/dodecahedron";
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_dodecahedron(gfx::mesh_vertex::decl);
				request.set_data(id, msh);
			}

			for(int i = 0; i < 6; ++i)
			{
				auto id = std::string("embedded:/icosphere") + std::to_string(i);
				auto& request = find_or_create_asset_entry<mesh>(id);
				auto msh = std::make_shared<mesh>();
				msh->create_icosphere(gfx::mesh_vertex::decl, i);
				request.set_data(id, msh);
			}
			
		}
		{
			auto storage = add<material>();
			storage->ext = extensions::material;
			storage->load_from_file = asset_reader::load_material_from_file;
			storage->save_to_file = asset_writer::write_material_to_file;
			{
				auto id = "embedded:/standard";
				auto& request = find_or_create_asset_entry<material>(id);
				auto mat = std::make_shared<standard_material>();
				request.set_data(id, mat);
			}
		}
		{
			auto storage = add<prefab>();
			storage->ext = extensions::prefab;
			storage->load_from_file = asset_reader::load_prefab_from_file;
		}
		{
			auto storage = add<scene>();
			storage->ext = extensions::scene;
			storage->load_from_file = asset_reader::load_scene_from_file;
		}

		return true;
	}
}