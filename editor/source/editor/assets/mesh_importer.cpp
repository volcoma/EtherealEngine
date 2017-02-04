#include "mesh_importer.h"
#include "graphics/graphics.h"

#include "core/math/math_includes.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "runtime/rendering/mesh.h"

void process_vertices(aiMesh* mesh, Mesh::LoadData& load_data)
{
	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = load_data.vertex_format.has(gfx::Attrib::Position);
	bool has_normal = load_data.vertex_format.has(gfx::Attrib::Normal);
	bool has_bitangent = load_data.vertex_format.has(gfx::Attrib::Bitangent);
	bool has_tangent = load_data.vertex_format.has(gfx::Attrib::Tangent);
	bool has_texcoord0 = load_data.vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_texcoord1 = load_data.vertex_format.has(gfx::Attrib::TexCoord1);
	auto nVertexStride = load_data.vertex_format.getStride();

	std::uint32_t nCurrentVertex = load_data.vertex_count;
	load_data.vertex_count += mesh->mNumVertices;
	load_data.vertex_data.resize(load_data.vertex_count * nVertexStride);

	std::uint8_t* current_vertex_ptr = &load_data.vertex_data[0] + nCurrentVertex * nVertexStride;

	for (size_t i = 0; i < mesh->mNumVertices; ++i, current_vertex_ptr += nVertexStride)
	{
		//position
		if (mesh->mVertices)
		{
			float position[4];
			std::memcpy(position, &mesh->mVertices[i], sizeof(math::vec3));

			if (has_position)
				gfx::vertexPack(position, false, gfx::Attrib::Position, load_data.vertex_format, current_vertex_ptr);
		}

		//tex coords
		if (mesh->mTextureCoords[0])
		{
			float textureCoords[4];
			std::memcpy(textureCoords, &mesh->mTextureCoords[0][i], sizeof(math::vec2));

			if (has_texcoord0)
				gfx::vertexPack(textureCoords, true, gfx::Attrib::TexCoord0, load_data.vertex_format, current_vertex_ptr);
	
		}

		////normals
		if (mesh->mNormals)
		{
			float normal[4];
			std::memcpy(normal, &mesh->mNormals[i], sizeof(math::vec3));

			if (has_normal)
				gfx::vertexPack(normal, true, gfx::Attrib::Normal, load_data.vertex_format, current_vertex_ptr);

		}

		//tangents
		if (mesh->mTangents)
		{
			float tangent[4];
			std::memcpy(tangent, &mesh->mTangents[i], sizeof(math::vec3));

			if (has_tangent)
				gfx::vertexPack(tangent, true, gfx::Attrib::Tangent, load_data.vertex_format, current_vertex_ptr);

		}

		//binormals
		if (mesh->mBitangents)
		{
			float bitangent[4];
			std::memcpy(bitangent, &mesh->mBitangents[i], sizeof(math::vec3));

			//negate bitangent;
			bitangent[0] = -bitangent[0];
			bitangent[1] = -bitangent[1];
			bitangent[2] = -bitangent[2];
			bitangent[3] = -bitangent[3];

			if (has_bitangent)
				gfx::vertexPack(bitangent, true, gfx::Attrib::Bitangent, load_data.vertex_format, current_vertex_ptr);

		}

	}
}

void process_faces(aiMesh* mesh, Mesh::LoadData& load_data)
{
	load_data.triangle_count += mesh->mNumFaces;

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		Mesh::Triangle triangle;

		triangle.data_group_id = mesh->mMaterialIndex;

		for (size_t j = 0; j < 3; ++j)
		{
			triangle.indices[j] = face.mIndices[j] + load_data.vertex_count;
		}

		load_data.triangle_data.push_back(triangle);

	}
}

void process_mesh(aiMesh* mesh, Mesh::LoadData& load_data)
{
	process_faces(mesh, load_data);
	process_vertices(mesh, load_data);
}

void process_material(aiMaterial* mat, Mesh::LoadData& load_data)
{

}

void process_meshes(const aiScene* scene, Mesh::LoadData& load_data)
{
	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		process_mesh(mesh, load_data);
	}
}

void process_materials(const aiScene* scene, Mesh::LoadData& load_data)
{
	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* mat = scene->mMaterials[i];
		process_material(mat, load_data);
	}
}

void process_imported_scene(const aiScene* scene, Mesh::LoadData& load_data)
{
	load_data.vertex_format = gfx::MeshVertex::decl;
	process_meshes(scene, load_data);
	process_materials(scene, load_data);
}


bool importer::load_mesh_data_from_file(const std::string& path, Mesh::LoadData& load_data)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path
		, aiProcess_ConvertToLeftHanded
		| aiProcess_CalcTangentSpace
		| aiProcess_GenSmoothNormals
		| aiProcess_JoinIdenticalVertices
		| aiProcess_ImproveCacheLocality
		| aiProcess_LimitBoneWeights
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_SplitLargeMeshes
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_SortByPType
		| aiProcess_FindDegenerates
		| aiProcess_FindInvalidData
		| aiProcess_FindInstances
		| aiProcess_ValidateDataStructure 
		| aiProcess_OptimizeMeshes
	);


	if (!scene)
		return false;

	process_imported_scene(scene, load_data);

	return true;
}
