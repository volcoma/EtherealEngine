#pragma once
#include "../../rendering/mesh.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(mesh::info)
{
	rttr::registration::class_<mesh::info>("info")
		.property_readonly("vertices", &mesh::info::vertices)(rttr::metadata("pretty_name", "Vertices"),
															  rttr::metadata("Tooltip", "Vertices count."))
		.property_readonly("primitives", &mesh::info::primitives)(
			rttr::metadata("pretty_name", "Primitives"), rttr::metadata("Tooltip", "Primitives count."))
		.property_readonly("subsets", &mesh::info::subsets)(rttr::metadata("pretty_name", "Subsets"),
															rttr::metadata("Tooltip", "Subsets count."));
}

namespace bgfx
{
SAVE_EXTERN(VertexDecl);
LOAD_EXTERN(VertexDecl);
}

SAVE_EXTERN(mesh::triangle);
LOAD_EXTERN(mesh::triangle);

SAVE_EXTERN(skin_bind_data::vertex_influence);
LOAD_EXTERN(skin_bind_data::vertex_influence);

SAVE_EXTERN(skin_bind_data::bone_influence);
LOAD_EXTERN(skin_bind_data::bone_influence);

SAVE_EXTERN(skin_bind_data);
LOAD_EXTERN(skin_bind_data);

SAVE_EXTERN(mesh::armature_node);
LOAD_EXTERN(mesh::armature_node);

SAVE_EXTERN(mesh::load_data);
LOAD_EXTERN(mesh::load_data);
