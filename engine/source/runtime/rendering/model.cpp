#include "model.h"
#include "material.h"
#include "mesh.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "program.h"
#include "core/math/math_includes.h"
#include "../assets/asset_manager.h"


Model::Model()
{
	auto am = core::get_subsystem<runtime::AssetManager>();
	am->load<Material>("engine_data:/materials/standard", false)
		.then([this](auto asset)
	{
		_default_material = asset;
	});
}

bool Model::is_valid() const
{
	return !_mesh_lods.empty();
}

AssetHandle<Mesh> Model::get_lod(std::uint32_t lod) const
{
	if (_mesh_lods.size() > lod)
	{
		auto lodMesh = _mesh_lods[lod];
		if (lodMesh)
			return lodMesh;

		for (unsigned int i = lod; i < _mesh_lods.size(); ++i)
		{
			auto lodMesh = _mesh_lods[i];
			if (lodMesh)
				return lodMesh;
		}
		for (unsigned int i = lod; i > 0; --i)
		{
			auto lodMesh = _mesh_lods[i];
			if (lodMesh)
				return lodMesh;
		}
	}
	return AssetHandle<Mesh>();
}

void Model::set_lod(AssetHandle<Mesh> mesh, std::uint32_t lod)
{
	if (lod >= _mesh_lods.size())
		_mesh_lods.resize(lod + 1);

	_mesh_lods[lod] = mesh;

	if(_materials.size() != mesh->groups.size())
		_materials.resize(mesh->groups.size(), _default_material);
}

void Model::set_material(AssetHandle<Material> material, std::uint32_t index)
{
	if (index >= _mesh_lods.size())
		_mesh_lods.resize(index + 1);

	_materials[index] = material;
}

const std::vector<AssetHandle<Mesh>>& Model::get_lods() const
{
	return _mesh_lods;
}

void Model::set_lods(const std::vector<AssetHandle<Mesh>>& lods)
{
	_mesh_lods = lods;
}


const std::vector<AssetHandle<Material>>& Model::get_materials() const
{
	return _materials;
}

void Model::set_materials(const std::vector<AssetHandle<Material>>& materials)
{
	_materials = materials;
}


AssetHandle<Material> Model::get_material_for_group(const size_t& group) const
{
	if (_materials.size() <= group)
		return AssetHandle<Material>();

	return _materials[group];
}

void Model::set_lod_max_distance(float distance)
{
	if (distance < _min_distance)
		distance = _min_distance;

	_max_distance = distance;
}

void Model::set_lod_min_distance(float distance)
{
	if (distance > _max_distance)
		distance = _max_distance;

	_min_distance = distance;
}

void Model::render(std::uint8_t id, const float* mtx, bool apply_cull, bool depth_write, bool depth_test, std::uint64_t extra_states, unsigned int lod, Program* user_program, std::function<void(Program&)> setup_params) const
{
	const auto mesh = get_lod(lod);
	if (!mesh)
		return;

	AssetHandle<Material> last_set_material;
	for (std::size_t i = 0; i < mesh->groups.size(); ++i)
	{
		const auto& group = mesh->groups[i];
		
		Program* program = user_program;
		AssetHandle<Material> mat = get_material_for_group(i);
		if (mat)
		{
			if (!user_program)
			{
				program = mat->get_program();
			}	
		}
		
		if (program)
		{
			program->begin_pass();
			setup_params(*program);
		}
		
		if (mat)
		{
			if (!user_program)
			{
				mat->submit();
			}

			extra_states |= mat->get_render_states(apply_cull, depth_write, depth_test);
		}

		if (program)
		{
			gfx::setTransform(mtx);
			gfx::setState(extra_states);

			gfx::setIndexBuffer(group.index_buffer->handle);
			gfx::setVertexBuffer(group.vertex_buffer->handle);

			gfx::submit(id, program->handle, 0, mat == last_set_material && i < (mesh->groups.size() - 1));
		}		

		last_set_material = mat;
	}
}
