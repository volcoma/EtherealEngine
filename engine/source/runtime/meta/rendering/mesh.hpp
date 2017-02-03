#pragma once
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "../../rendering/mesh.h"

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

SAVE(Mesh::LoadData)
{
	try_save(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_save(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_save(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_save(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_save(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
}

LOAD(Mesh::LoadData)
{
	try_load(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_load(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_load(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_load(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_load(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
}

