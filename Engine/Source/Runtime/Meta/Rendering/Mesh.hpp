#pragma once
#include "core/reflection/reflection.h"
#include "../../rendering/mesh.h"

REFLECT(MeshInfo)
{
	rttr::registration::class_<MeshInfo>("MeshInfo")
		.property_readonly("Vertices",
			&MeshInfo::vertices)
		.property_readonly("Indices",
			&MeshInfo::indices)
		.property_readonly("Primitives",
			&MeshInfo::primitives)
		;
}
