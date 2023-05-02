#include "mesh.hpp"

#include "../core/math/quaternion.hpp"
#include "../core/math/transform.hpp"

#include <core/serialization/binary_archive.h>

REFLECT(mesh::info)
{
	rttr::registration::class_<mesh::info>("info")
		.property_readonly("vertices", &mesh::info::vertices)(rttr::metadata("pretty_name", "Vertices"),
															  rttr::metadata("tooltip", "Vertices count."))
		.property_readonly("primitives", &mesh::info::primitives)(
			rttr::metadata("pretty_name", "Primitives"), rttr::metadata("tooltip", "Primitives count."))
		.property_readonly("subsets", &mesh::info::subsets)(rttr::metadata("pretty_name", "Subsets"),
															rttr::metadata("tooltip", "Subsets count."));
}

namespace bgfx
{
SAVE(VertexLayout)
{
	try_save(ar, cereal::make_nvp("hash", obj.m_hash));
	try_save(ar, cereal::make_nvp("stride", obj.m_stride));
	try_save(ar, cereal::make_nvp("offset", obj.m_offset));
	try_save(ar, cereal::make_nvp("attributes", obj.m_attributes));
}
SAVE_INSTANTIATE(VertexLayout, cereal::oarchive_binary_t);

LOAD(VertexLayout)
{
	try_load(ar, cereal::make_nvp("hash", obj.m_hash));
	try_load(ar, cereal::make_nvp("stride", obj.m_stride));
	try_load(ar, cereal::make_nvp("offset", obj.m_offset));
	try_load(ar, cereal::make_nvp("attributes", obj.m_attributes));
}
LOAD_INSTANTIATE(VertexLayout, cereal::iarchive_binary_t);
}

SAVE(mesh::triangle)
{
	try_save(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_save(ar, cereal::make_nvp("indices", obj.indices));
	try_save(ar, cereal::make_nvp("flags", obj.flags));
}
SAVE_INSTANTIATE(mesh::triangle, cereal::oarchive_binary_t);

LOAD(mesh::triangle)
{
	try_load(ar, cereal::make_nvp("data_group_id", obj.data_group_id));
	try_load(ar, cereal::make_nvp("indices", obj.indices));
	try_load(ar, cereal::make_nvp("flags", obj.flags));
}
LOAD_INSTANTIATE(mesh::triangle, cereal::iarchive_binary_t);

SAVE(skin_bind_data::vertex_influence)
{
	try_save(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_save(ar, cereal::make_nvp("weight", obj.weight));
}
SAVE_INSTANTIATE(skin_bind_data::vertex_influence, cereal::oarchive_binary_t);

LOAD(skin_bind_data::vertex_influence)
{
	try_load(ar, cereal::make_nvp("vertex_index", obj.vertex_index));
	try_load(ar, cereal::make_nvp("weight", obj.weight));
}
LOAD_INSTANTIATE(skin_bind_data::vertex_influence, cereal::iarchive_binary_t);

SAVE(skin_bind_data::bone_influence)
{
	try_save(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_save(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_save(ar, cereal::make_nvp("influences", obj.influences));
}
SAVE_INSTANTIATE(skin_bind_data::bone_influence, cereal::oarchive_binary_t);

LOAD(skin_bind_data::bone_influence)
{
	try_load(ar, cereal::make_nvp("bone_id", obj.bone_id));
	try_load(ar, cereal::make_nvp("bind_pose_transform", obj.bind_pose_transform));
	try_load(ar, cereal::make_nvp("influences", obj.influences));
}
LOAD_INSTANTIATE(skin_bind_data::bone_influence, cereal::iarchive_binary_t);

SAVE(skin_bind_data)
{
	try_save(ar, cereal::make_nvp("bones", obj.bones_));
}
SAVE_INSTANTIATE(skin_bind_data, cereal::oarchive_binary_t);

LOAD(skin_bind_data)
{
	try_load(ar, cereal::make_nvp("bones", obj.bones_));
}
LOAD_INSTANTIATE(skin_bind_data, cereal::iarchive_binary_t);

SAVE(mesh::armature_node)
{
	try_save(ar, cereal::make_nvp("name", obj.name));
	try_save(ar, cereal::make_nvp("local_transform", obj.local_transform));
	try_save(ar, cereal::make_nvp("children", obj.children));
}
SAVE_INSTANTIATE(mesh::armature_node, cereal::oarchive_binary_t);

LOAD(mesh::armature_node)
{
	try_load(ar, cereal::make_nvp("name", obj.name));
	try_load(ar, cereal::make_nvp("local_transform", obj.local_transform));
	try_load(ar, cereal::make_nvp("children", obj.children));
}
LOAD_INSTANTIATE(mesh::armature_node, cereal::iarchive_binary_t);

SAVE(mesh::load_data)
{
	try_save(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_save(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_save(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_save(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_save(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_save(ar, cereal::make_nvp("material_count", obj.material_count));
	try_save(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_save(ar, cereal::make_nvp("root_node", obj.root_node));
}
SAVE_INSTANTIATE(mesh::load_data, cereal::oarchive_binary_t);

LOAD(mesh::load_data)
{
	try_load(ar, cereal::make_nvp("vertex_format", obj.vertex_format));
	try_load(ar, cereal::make_nvp("vertex_count", obj.vertex_count));
	try_load(ar, cereal::make_nvp("vertex_data", obj.vertex_data));
	try_load(ar, cereal::make_nvp("triangle_count", obj.triangle_count));
	try_load(ar, cereal::make_nvp("triangle_data", obj.triangle_data));
	try_load(ar, cereal::make_nvp("material_count", obj.material_count));
	try_load(ar, cereal::make_nvp("skin_data", obj.skin_data));
	try_load(ar, cereal::make_nvp("root_node", obj.root_node));
}
LOAD_INSTANTIATE(mesh::load_data, cereal::iarchive_binary_t);
