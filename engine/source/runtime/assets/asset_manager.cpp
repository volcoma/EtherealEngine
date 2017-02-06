#include "asset_manager.h"
#include "asset_reader.h"
#include "asset_writer.h"
#include "asset_extensions.h"
#include "graphics/graphics.h"

#include "../rendering/mesh.h"
#include "../rendering/material.h"

namespace runtime
{
	bool AssetManager::initialize()
	{
		{
			auto storage = add<Shader>();
			storage->ext = extensions::shader;
			storage->load_from_file = AssetReader::load_shader_from_file;
			storage->load_from_memory = AssetReader::load_shader_from_memory;
		}
		{
			auto storage = add<Texture>();
			storage->ext = extensions::texture;
			storage->load_from_file = AssetReader::load_texture_from_file;
		}
		{
			auto storage = add<Mesh>();
			storage->ext = extensions::mesh;
			storage->load_from_file = AssetReader::load_mesh_from_file;
			{
				auto id = "embedded:/sphere";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_sphere(gfx::MeshVertex::decl, 0.5f, 20, 20, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/cube";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_cube(gfx::MeshVertex::decl, 1.0f, 1.0f, 1.0f, 1, 1, 1, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/plane";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_cube(gfx::MeshVertex::decl, 10.0f, 0.01f, 10.0f, 1, 1, 1, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/cylinder";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_cylinder(gfx::MeshVertex::decl, 0.5f, 2.0f, 20, 20, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/capsule";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_capsule(gfx::MeshVertex::decl, 0.5f, 2.0f, 20, 20, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/cone";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_cone(gfx::MeshVertex::decl, 0.5f, 0.0f, 2, 20, 20, false, MeshCreateOrigin::Bottom);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/torus";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_torus(gfx::MeshVertex::decl, 1.0f, 0.5f, 20, 20, false, MeshCreateOrigin::Center);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/teapot";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_teapot(gfx::MeshVertex::decl);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/tetrahedron";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_tetrahedron(gfx::MeshVertex::decl);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/octahedron";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_octahedron(gfx::MeshVertex::decl);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/icosahedron";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_icosahedron(gfx::MeshVertex::decl);
				request.set_data(id, mesh);
			}
			{
				auto id = "embedded:/dodecahedron";
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_dodecahedron(gfx::MeshVertex::decl);
				request.set_data(id, mesh);
			}

			for(int i = 0; i < 6; ++i)
			{
				auto id = std::string("embedded:/icosphere") + std::to_string(i);
				auto& request = find_or_create_asset_entry<Mesh>(id);
				auto mesh = std::make_shared<Mesh>();
				mesh->create_icosphere(gfx::MeshVertex::decl, i);
				request.set_data(id, mesh);
			}
			
		}
		{
			auto storage = add<Material>();
			storage->ext = extensions::material;
			storage->load_from_file = AssetReader::load_material_from_file;
			storage->save_to_file = AssetWriter::write_material_to_file;
			{
				auto id = "embedded:/standard";
				auto& request = find_or_create_asset_entry<Material>(id);
				auto mat = std::make_shared<StandardMaterial>();
				request.set_data(id, mat);
			}
		}
		{
			auto storage = add<Prefab>();
			storage->ext = extensions::prefab;
			storage->load_from_file = AssetReader::load_prefab_from_file;
		}
		{
			auto storage = add<Scene>();
			storage->ext = extensions::scene;
			storage->load_from_file = AssetReader::load_scene_from_file;
		}

		return true;
	}
}