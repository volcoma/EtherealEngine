#include "asset_manager.h"
#include "asset_reader.h"
#include "asset_writer.h"
#include "asset_extensions.h"
#include "graphics/graphics.h"

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
			//storage->loadFromMemory = AssetReader::loadTextureFromMemory;
		}
		{
			auto storage = add<Mesh>();
			storage->ext = extensions::mesh;
			storage->load_from_file = AssetReader::load_mesh_from_file;
			//storage->loadFromMemory = AssetReader::loadMeshFromMemory;
		}
		{
			auto storage = add<Material>();
			storage->ext = extensions::material;
			storage->load_from_file = AssetReader::load_material_from_file;
			storage->save_to_file = AssetWriter::write_material_to_file;
			//storage->loadFromMemory = AssetReader::loadMaterialFromMemory;
		}
		{
			auto storage = add<Prefab>();
			storage->ext = extensions::prefab;
			storage->load_from_file = AssetReader::load_prefab_from_file;
		}

		return true;
	}
}