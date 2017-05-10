#pragma once
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "../../rendering/mesh.h"
#include "../math/transform.hpp"

REFLECT(mesh::info)
{
	rttr::registration::class_<mesh::info>("info")
		.property_readonly("Vertices",
			&mesh::info::vertices)
		.property_readonly("Primitives",
			&mesh::info::primitives)
		.property_readonly("Subsets",
			&mesh::info::subsets)
		;
}

namespace bgfx
{
	SAVE(VertexDecl)
	{
		try_save(ar, cereal::make_nvp("hash", obj.m_hash));
		try_save(ar, cereal::make_nvp("stride", obj.m_stride));
		try_save(ar, cereal::make_nvp("offset", obj.m_offset));
		try_save(ar, cereal::make_nvp("attributes", obj.m_attributes));
	}

	LOAD(VertexDecl)
	{
		try_load(ar, cereal::make_nvp("hash", obj.m_hash));
		try_load(ar, cereal::make_nvp("stride", obj.m_stride));
		try_load(ar, cereal::make_nvp("offset", obj.m_offset));
		try_load(ar, cereal::make_nvp("attributes", obj.m_attributes));
	}
}


SAVE(mesh::triangle)
{
	try_save(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_save(ar, cereal::make_nvp("indices", obj.indices));
	try_save(ar, cereal::make_nvp("flags", obj.flags));
}

LOAD(mesh::triangle)
{
	try_load(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_load(ar, cereal::make_nvp("indices", obj.indices));
	try_load(ar, cereal::make_nvp("flags", obj.flags));
}

SAVE(skin_bind_data::vertex_influence)
{
	try_save(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_save(ar, cereal::make_nvp("weight", obj.weight));
}

LOAD(skin_bind_data::vertex_influence)
{
	try_load(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_load(ar, cereal::make_nvp("weight", obj.weight));
}

SAVE(skin_bind_data::bone_influence)
{
	try_save(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_save(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_save(ar, cereal::make_nvp("influences", obj.influences));
}

LOAD(skin_bind_data::bone_influence)
{
	try_load(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_load(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_load(ar, cereal::make_nvp("influences", obj.influences));
}

SAVE(skin_bind_data)
{
	try_save(ar, cereal::make_nvp("bones", obj._bones));
}

LOAD(skin_bind_data)
{
	try_load(ar, cereal::make_nvp("bones", obj._bones));
}

SAVE(mesh::armature_node)
{
	try_save(ar, cereal::make_nvp("name", obj.name));
	try_save(ar, cereal::make_nvp("transform", obj.transform));
	try_save(ar, cereal::make_nvp("children", obj.children));
}

LOAD(mesh::armature_node)
{
	try_load(ar, cereal::make_nvp("name", obj.name));
	try_load(ar, cereal::make_nvp("transform", obj.transform));
	try_load(ar, cereal::make_nvp("children", obj.children));
}

SAVE(mesh::load_data)
{
	try_save(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_save(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_save(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_save(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_save(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_save(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_save(ar, cereal::make_nvp("root_node", obj.root_node));
}

LOAD(mesh::load_data)
{
	try_load(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_load(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_load(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_load(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_load(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_load(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_load(ar, cereal::make_nvp("root_node", obj.root_node));
}

