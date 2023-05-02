#pragma once
#include "../../rendering/mesh.h"

#include <core/reflection/reflection.h>
#include <core/serialization/serialization.h>

REFLECT_EXTERN(mesh::info);

namespace bgfx
{
SAVE_EXTERN(VertexLayout);
LOAD_EXTERN(VertexLayout);
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
