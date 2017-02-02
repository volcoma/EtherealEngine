#include "mesh_importer.h"
#include "graphics/graphics.h"

#include "core/math/math_includes.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "runtime/rendering/mesh.h"

void process_vertices(aiMesh* mesh, Mesh::LoadData& loadData)
{
	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = loadData.vertex_format.has(gfx::Attrib::Position);
	bool has_normal = loadData.vertex_format.has(gfx::Attrib::Normal);
	bool has_bitangent = loadData.vertex_format.has(gfx::Attrib::Bitangent);
	bool has_tangent = loadData.vertex_format.has(gfx::Attrib::Tangent);
	bool has_texcoord0 = loadData.vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_texcoord1 = loadData.vertex_format.has(gfx::Attrib::TexCoord1);
	auto nVertexStride = loadData.vertex_format.getStride();

	std::uint32_t nCurrentVertex = loadData.vertex_count;
	loadData.vertex_count += mesh->mNumVertices;
	loadData.vertex_data.resize(loadData.vertex_count * nVertexStride);

	std::uint8_t* pCurrentVertex = &loadData.vertex_data[0] + nCurrentVertex * nVertexStride;

	for (size_t i = 0; i < mesh->mNumVertices; ++i, pCurrentVertex += nVertexStride)
	{
		//position
		if (mesh->mVertices)
		{
			float position[4];
			std::memcpy(position, &mesh->mVertices[i], sizeof(math::vec3));

			if (has_position)
				gfx::vertexPack(position, false, gfx::Attrib::Position, loadData.vertex_format, pCurrentVertex);
		}

		//tex coords
		if (mesh->mTextureCoords[0])
		{
			float textureCoords[4];
			std::memcpy(textureCoords, &mesh->mTextureCoords[0][i], sizeof(math::vec2));

			if (has_texcoord0)
				gfx::vertexPack(textureCoords, true, gfx::Attrib::TexCoord0, loadData.vertex_format, pCurrentVertex);
	
		}

		////normals
		if (mesh->mNormals)
		{
			float normal[4];
			std::memcpy(normal, &mesh->mNormals[i], sizeof(math::vec3));

			if (has_normal)
				gfx::vertexPack(normal, true, gfx::Attrib::Normal, loadData.vertex_format, pCurrentVertex);

		}

		//tangents
		if (mesh->mTangents)
		{
			float tangent[4];
			std::memcpy(tangent, &mesh->mTangents[i], sizeof(math::vec3));

			if (has_tangent)
				gfx::vertexPack(tangent, true, gfx::Attrib::Tangent, loadData.vertex_format, pCurrentVertex);

		}

		//binormals
		if (mesh->mBitangents)
		{
			float bitangent[4];
			std::memcpy(bitangent, &mesh->mBitangents[i], sizeof(math::vec3));

			if (has_bitangent)
				gfx::vertexPack(bitangent, true, gfx::Attrib::Bitangent, loadData.vertex_format, pCurrentVertex);

		}

	}
}

void process_faces(aiMesh* mesh, Mesh::LoadData& loadData)
{
	loadData.triangle_count += mesh->mNumFaces;

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		Mesh::Triangle triangle;

		triangle.data_group_id = mesh->mMaterialIndex;

		for (size_t j = 0; j < 3; ++j)
		{
			triangle.indices[j] = face.mIndices[j] + loadData.vertex_count;
		}

		loadData.triangle_data.push_back(triangle);

	}
}
//-----------------------------------------------------------------------------
//  Name : ProcessSubset ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
void process_mesh(aiMesh* mesh, Mesh::LoadData& loadData)
{
	process_faces(mesh, loadData);
	process_vertices(mesh, loadData);
}

void process_material(aiMaterial* mat, Mesh::LoadData& loadData)
{

}

void process_meshes(const aiScene* scene, Mesh::LoadData& loadData)
{
	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		process_mesh(mesh, loadData);
	}
}

void process_materials(const aiScene* scene, Mesh::LoadData& loadData)
{
	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* mat = scene->mMaterials[i];
		process_material(mat, loadData);
	}
}

//-----------------------------------------------------------------------------
//  Name : process_imported_scene ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
void process_imported_scene(const aiScene* scene, Mesh::LoadData& loadData)
{

	loadData.vertex_format = gfx::MeshVertex::decl;
	process_meshes(scene, loadData);
	process_materials(scene, loadData);
}


bool importer::load_mesh_data_from_file(const std::string& path, Mesh::LoadData& loadData)
{
	Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(
		path
		, aiProcess_ConvertToLeftHanded
		| aiProcessPreset_TargetRealtime_MaxQuality
	);


	if (!pScene)
		return false;

	process_imported_scene(pScene, loadData);

	return true;
}
