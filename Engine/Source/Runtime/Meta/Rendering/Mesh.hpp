#pragma once
#include "Core/reflection/reflection.h"
#include "../../Rendering/Mesh.h"

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
