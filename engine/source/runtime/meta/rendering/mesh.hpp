#pragma once
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "../../rendering/mesh.h"
#include "../math/transform.hpp"

REFLECT(Mesh::Info)
{
	rttr::registration::class_<Mesh::Info>("Info")
		.property_readonly("Vertices",
			&Mesh::Info::vertices)
		.property_readonly("Primitives",
			&Mesh::Info::primitives)
		.property_readonly("Subsets",
			&Mesh::Info::subsets)
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


SAVE(Mesh::Triangle)
{
	try_save(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_save(ar, cereal::make_nvp("indices", obj.indices));
	try_save(ar, cereal::make_nvp("flags", obj.flags));
}

LOAD(Mesh::Triangle)
{
	try_load(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_load(ar, cereal::make_nvp("indices", obj.indices));
	try_load(ar, cereal::make_nvp("flags", obj.flags));
}

SAVE(SkinBindData::VertexInfluence)
{
	try_save(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_save(ar, cereal::make_nvp("weight", obj.weight));
}

LOAD(SkinBindData::VertexInfluence)
{
	try_load(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_load(ar, cereal::make_nvp("weight", obj.weight));
}

SAVE(SkinBindData::BoneInfluence)
{
	try_save(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_save(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_save(ar, cereal::make_nvp("influences", obj.influences));
}

LOAD(SkinBindData::BoneInfluence)
{
	try_load(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_load(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_load(ar, cereal::make_nvp("influences", obj.influences));
}

SAVE(SkinBindData)
{
	try_save(ar, cereal::make_nvp("bones", obj._bones));
}

LOAD(SkinBindData)
{
	try_load(ar, cereal::make_nvp("bones", obj._bones));
}

SAVE(Mesh::ArmatureNode)
{
	try_save(ar, cereal::make_nvp("name", obj.name));
	try_save(ar, cereal::make_nvp("transform", obj.transform));
	try_save(ar, cereal::make_nvp("children", obj.children));
}

LOAD(Mesh::ArmatureNode)
{
	try_load(ar, cereal::make_nvp("name", obj.name));
	try_load(ar, cereal::make_nvp("transform", obj.transform));
	try_load(ar, cereal::make_nvp("children", obj.children));
}

SAVE(Mesh::LoadData)
{
	try_save(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_save(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_save(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_save(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_save(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_save(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_save(ar, cereal::make_nvp("root_node", obj.root_node));
}

LOAD(Mesh::LoadData)
{
	try_load(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_load(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_load(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_load(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_load(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_load(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_load(ar, cereal::make_nvp("root_node", obj.root_node));
}

