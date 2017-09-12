#include "model.h"
#include "../assets/asset_manager.h"
#include "core/math/math_includes.h"
#include "index_buffer.h"
#include "material.h"
#include "mesh.h"
#include "program.h"
#include "vertex_buffer.h"

model::model()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto standard = am.load<material>("embedded:/standard");
	_default_material = standard.get();
}

bool model::is_valid() const
{
	return !_mesh_lods.empty();
}

asset_handle<mesh> model::get_lod(std::uint32_t lod) const
{
	if(_mesh_lods.size() > lod)
	{
		auto lodMesh = _mesh_lods[lod];
		if(lodMesh)
			return lodMesh;

		for(unsigned int i = lod; i < _mesh_lods.size(); ++i)
		{
			auto lodMesh = _mesh_lods[i];
			if(lodMesh)
				return lodMesh;
		}
		for(unsigned int i = lod; i > 0; --i)
		{
			auto lodMesh = _mesh_lods[i];
			if(lodMesh)
				return lodMesh;
		}
	}
	return asset_handle<mesh>();
}

void model::set_lod(asset_handle<mesh> mesh, std::uint32_t lod)
{
	if(lod >= _mesh_lods.size())
    {
		_mesh_lods.resize(lod + 1);
        recalulate_lod_limits();
    }
	_mesh_lods[lod] = mesh;

	if(_materials.size() != mesh->get_subset_count())
	{
		_materials.resize(mesh->get_subset_count(), _default_material);
	}
}

void model::set_material(asset_handle<material> material, std::uint32_t index)
{
	if(index >= _mesh_lods.size())
		_mesh_lods.resize(index + 1);

	_materials[index] = material;
}

const std::vector<asset_handle<mesh>>& model::get_lods() const
{
	return _mesh_lods;
}

void model::set_lods(const std::vector<asset_handle<mesh>>& lods)
{
	auto sz1 = lods.size();
	auto sz2 = _mesh_lods.size();

	_mesh_lods = lods;

	if(sz1 != sz2)
		recalulate_lod_limits();

	if(_mesh_lods.size() > 0)
	{
		auto& mesh = _mesh_lods[0];
		if(mesh)
		{
			if(_materials.size() != mesh->get_subset_count())
				_materials.resize(mesh->get_subset_count(), _default_material);
		}
	}
}

const std::vector<asset_handle<material>>& model::get_materials() const
{
	return _materials;
}

void model::set_materials(const std::vector<asset_handle<material>>& materials)
{
	_materials = materials;
}

asset_handle<material> model::get_material_for_group(const size_t& group) const
{
	if(_materials.size() <= group)
		return asset_handle<material>();

	return _materials[group];
}

void model::set_lod_limits(const std::vector<urange>& limits)
{
	_lod_limits = limits;
}

void model::render(std::uint8_t id, const math::transform& world_transform,
				   const std::vector<math::transform>& bone_transforms, bool apply_cull, bool depth_write,
				   bool depth_test, std::uint64_t extra_states, unsigned int lod, program* user_program,
				   std::function<void(program&)> setup_params) const
{
	const auto mesh = get_lod(lod);
	if(!mesh)
		return;

	auto render_subset = [this, &mesh](std::uint8_t id, bool skinned, std::uint32_t group_id,
									   const float* mtx, std::uint32_t count, bool apply_cull,
									   bool depth_write, bool depth_test, std::uint64_t extra_states,
									   program* user_program, std::function<void(program&)> setup_params) {

		bool valid_program = false;
		program* program = user_program;
		asset_handle<material> mat = get_material_for_group(group_id);

		if(mat)
		{
			mat->skinned = skinned;
			if(!user_program)
			{
				program = mat->get_program();
			}
		}

		if(program)
		{
			valid_program = program->begin_pass();
			if(valid_program)
				setup_params(*program);
		}

		if(valid_program)
		{
			if(mat)
			{
				if(!user_program)
				{
					mat->submit();
				}

				extra_states |= mat->get_render_states(apply_cull, depth_write, depth_test);
			}

			if(mtx != nullptr)
				gfx::setTransform(mtx, static_cast<std::uint16_t>(count));

			gfx::setState(extra_states);

			mesh->bind_render_buffers_for_subset(group_id);

			gfx::submit(id, program->handle);
		}

	};

	const auto& skin_data = mesh->get_skin_bind_data();

	// Has skinning data?
	if(skin_data.has_bones() && bone_transforms.empty() == false)
	{
		// Process each palette in the skin with a matching attribute.
		const auto& palettes = mesh->get_bone_palettes();
		for(const auto& palette : palettes)
		{
			// Apply the bone palette.
			auto skinning_matrices = palette.get_skinning_matrices(bone_transforms, skin_data, false);
			// auto max_blend_index = palette.get_maximum_blend_index();

			auto data_group = palette.get_data_group();
			render_subset(id, true, data_group, reinterpret_cast<float*>(&skinning_matrices[0]),
						  std::uint32_t(skinning_matrices.size()), apply_cull, depth_write, depth_test,
						  extra_states, user_program, setup_params);

		} // Next Palette
	}
	else
	{
		for(std::size_t i = 0; i < mesh->get_subset_count(); ++i)
		{
			render_subset(id, false, std::uint32_t(i), world_transform, 1, apply_cull, depth_write,
						  depth_test, extra_states, user_program, setup_params);
		}
	}
}

void model::recalulate_lod_limits()
{
	float upper_limit = 100.0f;
	_lod_limits.clear();
	_lod_limits.reserve(_mesh_lods.size());

	for(size_t i = 0; i < _mesh_lods.size(); ++i)
	{
		float lower_limit = 0.0f;
        
        if(_mesh_lods.size() - 1 != i)
            lower_limit = upper_limit * (0.6f - ((i)*0.1f));
        
		_lod_limits.emplace_back(urange(urange::value_type(lower_limit), urange::value_type(upper_limit)));
		upper_limit = lower_limit;
	}
}
