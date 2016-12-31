#include "asset_manager.h"
#include "asset_reader.h"
#include "asset_writer.h"
#include "graphics/graphics.h"

void AssetManager::setup()
{
	{
		auto storage = add<Shader>();
		storage->loadFromFile = AssetReader::loadShaderFromFile;
		storage->loadFromMemory = AssetReader::loadShaderFromMemory;
		storage->subdir = "runtime/";
		switch (gfx::getRendererType())
		{
		case gfx::RendererType::Direct3D9:
			storage->platform = "dx9/";
			break;
		case gfx::RendererType::Direct3D11:
		case gfx::RendererType::Direct3D12:
			storage->platform = "dx11/";
			break;

		case gfx::RendererType::OpenGL:
			storage->platform = "glsl/";
			break;

		case gfx::RendererType::Metal:
			storage->platform = "metal/";
			break;

		case gfx::RendererType::OpenGLES:
			storage->platform = "gles/";
			break;

		default:
			break;
		}

	}
	{
		auto storage = add<Texture>();
		storage->subdir = "runtime/";
		storage->loadFromFile = AssetReader::loadTextureFromFile;
		//storage->loadFromMemory = AssetReader::loadTextureFromMemory;
	}
	{
		auto storage = add<Mesh>();
		storage->subdir = "runtime/";
		storage->loadFromFile = AssetReader::loadMeshFromFile;
		//storage->loadFromMemory = AssetReader::loadMeshFromMemory;
	}
	{
		auto storage = add<Material>();
		storage->subdir = "";
		storage->loadFromFile = AssetReader::loadMaterialFromFile;
		storage->saveToFile = AssetWriter::saveMaterialToFile;
		//storage->loadFromMemory = AssetReader::loadMaterialFromMemory;
	}
	{
		auto storage = add<Prefab>();
		storage->subdir = "";
		storage->loadFromFile = AssetReader::loadPrefabFromFile;
	}
}
