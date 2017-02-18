#pragma once
#include "runtime/rendering/mesh.h"

namespace importer
{
	bool load_mesh_data_from_file(const std::string& path, Mesh::LoadData& loadData);
}