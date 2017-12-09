#pragma once
#include "runtime/animation/animation.h"
#include "runtime/rendering/mesh.h"

namespace importer
{
bool load_mesh_data_from_file(const std::string& path, mesh::load_data& load_data,
							  std::vector<animation>& animations);
}
