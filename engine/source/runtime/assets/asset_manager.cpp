#include "asset_manager.h"
#include "asset_reader.h"
#include "asset_writer.h"
#include "graphics/graphics.h"

namespace runtime
{
	void AssetManager::setup()
	{
		{
			auto storage = add<Shader>();
			storage->load_from_file = AssetReader::load_shader_from_file;
			storage->load_from_memory = AssetReader::load_shader_from_memory;
// 			storage->subdir = "compiled/";
// 			switch (gfx::getRendererType())
// 			{
// 			case gfx::RendererType::Direct3D9:
// 				storage->platform = "dx9/";
// 				break;
// 			case gfx::RendererType::Direct3D11:
// 			case gfx::RendererType::Direct3D12:
// 				storage->platform = "dx11/";
// 				break;
// 
// 			case gfx::RendererType::OpenGL:
// 				storage->platform = "glsl/";
// 				break;
// 
// 			case gfx::RendererType::Metal:
// 				storage->platform = "metal/";
// 				break;
// 
// 			case gfx::RendererType::OpenGLES:
// 				storage->platform = "gles/";
// 				break;
// 
// 			default:
// 				break;
// 			}

		}
		{
			auto storage = add<Texture>();
			storage->subdir = "";
			storage->load_from_file = AssetReader::load_texture_from_file;
			//storage->loadFromMemory = AssetReader::loadTextureFromMemory;
		}
		{
			auto storage = add<Mesh>();
			storage->subdir = "";
			storage->load_from_file = AssetReader::load_mesh_from_file;
			//storage->loadFromMemory = AssetReader::loadMeshFromMemory;
		}
		{
			auto storage = add<Material>();
			storage->subdir = "";
			storage->load_from_file = AssetReader::load_material_from_file;
			storage->save_to_file = AssetWriter::write_material_to_file;
			//storage->loadFromMemory = AssetReader::loadMaterialFromMemory;
		}
		{
			auto storage = add<Prefab>();
			storage->subdir = "";
			storage->load_from_file = AssetReader::load_prefab_from_file;
		}
	}
}