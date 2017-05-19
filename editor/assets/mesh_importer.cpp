#include "mesh_importer.h"
#include "graphics/graphics.h"

#include "core/math/math_includes.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "runtime/rendering/mesh.h"


math::transform process_matrix(const aiMatrix4x4& assimp_matrix)
{
	math::transform matrix;

	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = 0; j < 4; ++j)
		{
			matrix[i][j] = assimp_matrix[i][j];
		}
	}
	return matrix;
}

void process_vertices(aiMesh* mesh, mesh::load_data& load_data)
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
		if (mesh->mVertices && has_position)
		{
			float position[4];
			std::memcpy(position, &mesh->mVertices[i], sizeof(math::vec3));

			if (has_position)
				gfx::vertexPack(position, false, gfx::Attrib::Position, load_data.vertex_format, current_vertex_ptr);
		}

		//tex coords
		if (mesh->mTextureCoords[0] && has_texcoord0)
		{
			float textureCoords[4];
			std::memcpy(textureCoords, &mesh->mTextureCoords[0][i], sizeof(math::vec2));

			if (has_texcoord0)
				gfx::vertexPack(textureCoords, true, gfx::Attrib::TexCoord0, load_data.vertex_format, current_vertex_ptr);
	
		}

		////normals
		math::vec4 normal;
		if (mesh->mNormals && has_normal)
		{
			std::memcpy(math::value_ptr(normal), &mesh->mNormals[i], sizeof(math::vec3));

			if (has_normal)
				gfx::vertexPack(math::value_ptr(normal), true, gfx::Attrib::Normal, load_data.vertex_format, current_vertex_ptr);

		}

		math::vec4 tangent;
		//tangents
		if (mesh->mTangents && has_tangent)
		{
			std::memcpy(math::value_ptr(tangent), &mesh->mTangents[i], sizeof(math::vec3));
			tangent.w = 1.0f;
			if (has_tangent)
				gfx::vertexPack(math::value_ptr(tangent), true, gfx::Attrib::Tangent, load_data.vertex_format, current_vertex_ptr);

		}

		//binormals
		math::vec4 bitangent;
		if (mesh->mBitangents && has_bitangent)
		{
			std::memcpy(math::value_ptr(bitangent), &mesh->mBitangents[i], sizeof(math::vec3));
			float handedness = math::dot(math::vec3(bitangent), math::normalize(math::cross(math::vec3(normal), math::vec3(tangent))));
			tangent.w = handedness;

			if (has_bitangent)
				gfx::vertexPack(math::value_ptr(bitangent), true, gfx::Attrib::Bitangent, load_data.vertex_format, current_vertex_ptr);

		}

	}
}

void process_faces(aiMesh* mesh, mesh::load_data& load_data)
{
	load_data.triangle_count += mesh->mNumFaces;

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		mesh::triangle triangle;

		triangle.data_group_id = mesh->mMaterialIndex;

		for (size_t j = 0; j < 3; ++j)
		{
			triangle.indices[j] = face.mIndices[j] + load_data.vertex_count;
		}

		load_data.triangle_data.push_back(triangle);

	}
}

void process_bones(aiMesh* mesh, mesh::load_data& load_data)
{
	if (mesh->mBones)
	{
		skin_bind_data::bone_influence_array_t bone_influences;
		for (size_t i = 0; i < mesh->mNumBones; ++i)
		{
			aiBone* assimp_bone = mesh->mBones[i];
			skin_bind_data::bone_influence bone_influence;
			bone_influence.bone_id = assimp_bone->mName.C_Str();
			const aiMatrix4x4& assimp_matrix = assimp_bone->mOffsetMatrix;
			math::transform matrix = process_matrix(assimp_matrix);
			bone_influence.bind_pose_transform = math::transpose(matrix);

			for (size_t j = 0; j < assimp_bone->mNumWeights; ++j)
			{
				aiVertexWeight assimp_influence = assimp_bone->mWeights[j];

				skin_bind_data::vertex_influence influence;
				influence.vertex_index = assimp_influence.mVertexId;
				influence.weight = assimp_influence.mWeight;

				bone_influence.influences.push_back(influence);
			}

			bone_influences.push_back(std::move(bone_influence));
		}


		for (size_t i = 0; i < bone_influences.size(); ++i)
		{
			const auto& bone_influence_to_insert = bone_influences[i];
			bool exist = false;
			const auto& bones = load_data.skin_data.get_bones();
			for (auto& bone: bones)
			{
				if (bone.bone_id == bone_influence_to_insert.bone_id)
				{
					exist = true;
					break;
				}

			}
			if (!exist)
				load_data.skin_data.add_bone(bone_influence_to_insert);
		}
		bone_influences.clear();
	}
}

void process_mesh(aiMesh* mesh, mesh::load_data& load_data)
{
	process_faces(mesh, load_data);
	process_vertices(mesh, load_data);
	process_bones(mesh, load_data);
}

void process_material(aiMaterial* mat, mesh::load_data& load_data)
{
	auto mat_index = load_data.materials.size();
	load_data.materials.push_back({});
	// Build the material Diffuse, Specular, NormalMap and DisplacementColor surfaces.
	aiColor3D color;
	float opacity = 0.0f;

	mesh::load_data::load_material data;

	auto is_not_black = [](aiColor3D color)
	{
		return color.r != 0 || color.g != 0 || color.b != 0;
	};

	auto process_color = [](const aiColor3D& c)
	{
		return math::color(c.r, c.g, c.b, 1.0f);
	};

	if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) // always keep black color for diffuse
	{
		data.base_color = process_color(color);
	}
	if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS && is_not_black(color))
	{
		data.emissive_color = process_color(color);
	}
	if (mat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
	{
		data.base_color.value.a = opacity;
	}

	auto get_texture = [](std::size_t mat_index, aiMaterial* mat, mesh::load_data& load_data, aiTextureType t, mesh::load_data::load_material::texture_type tt)
	{
		const auto textures = mat->GetTextureCount(t);
		if (textures > 0)
		{
			aiString path;
			aiTextureMapping mapping;
			unsigned int index = 0;
			float blend = 0.0f;
			aiTextureOp textureOp;
			aiTextureMapMode mapMode;
			if (AI_SUCCESS == mat->GetTexture(t, 0, &path, &mapping, &index, &blend, &textureOp, &mapMode))
			{
				std::string relative_path = path.C_Str();
				load_data.materials[mat_index].textures[t] = relative_path;
			}
		}
	};

	get_texture(mat_index, mat, load_data, aiTextureType_DIFFUSE, mesh::load_data::load_material::BaseColor);
	get_texture(mat_index, mat, load_data, aiTextureType_NORMALS, mesh::load_data::load_material::Normal);
	get_texture(mat_index, mat, load_data, aiTextureType_EMISSIVE, mesh::load_data::load_material::Emissive);
}


void process_meshes(const aiScene* scene, mesh::load_data& load_data)
{
	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		process_mesh(mesh, load_data);
	}
}

void process_materials(const aiScene* scene, mesh::load_data& load_data)
{
	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* mat = scene->mMaterials[i];
		process_material(mat, load_data);
	}
}

void process_node(aiNode* node, mesh::armature_node& armature_node)
{
	armature_node.children.resize(node->mNumChildren);

	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		aiNode* child_node = node->mChildren[i];
		auto child_armature = std::make_unique<mesh::armature_node>();
		child_armature->name = child_node->mName.C_Str();

		const aiMatrix4x4& assimp_matrix = child_node->mTransformation;
		math::transform matrix = process_matrix(assimp_matrix);
		child_armature->transform = math::transpose(matrix);

		armature_node.children[i] = std::move(child_armature);
		process_node(child_node, *armature_node.children[i].get());
	}
}

void process_nodes(const aiScene* scene, mesh::load_data& load_data)
{
	aiNode* root = scene->mRootNode->mChildren[0];
	load_data.root_node = std::make_unique<mesh::armature_node>();
	process_node(root, *load_data.root_node.get());

}

void process_imported_scene(const aiScene* scene, mesh::load_data& load_data)
{
	load_data.vertex_format = gfx::mesh_vertex::decl;
	process_meshes(scene, load_data);
	process_materials(scene, load_data);
	process_nodes(scene, load_data);
}


bool importer::load_mesh_data_from_file(const std::string& path, mesh::load_data& load_data)
{
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool("GLOB_MEASURE_TIME", true);

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
	{
		APPLOG_ERROR(importer.GetErrorString());
		return false;
	}
	APPLOG_INFO("Imported {0}", path);
	process_imported_scene(scene, load_data);

	return true;
}
