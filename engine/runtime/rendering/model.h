#pragma once

#include "../assets/asset_handle.h"
#include "core/math/math_includes.h"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"
#include <vector>

struct Group;
class mesh;
struct program;
class material;

class model
{
public:
	REFLECTABLE(model)
	SERIALIZABLE(model)

	//-----------------------------------------------------------------------------
	//  Name : model ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	model();

	//-----------------------------------------------------------------------------
	//  Name : ~model (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~model() = default;

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	//  Name : get_lod ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	asset_handle<mesh> get_lod(std::uint32_t lod) const;

	//-----------------------------------------------------------------------------
	//  Name : set_lod ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lod(asset_handle<mesh> mesh, std::uint32_t lod);

	//-----------------------------------------------------------------------------
	//  Name : set_material ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_material(asset_handle<material> material, std::uint32_t index);

	//-----------------------------------------------------------------------------
	//  Name : get_lods ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<asset_handle<mesh>>& get_lods() const;

	//-----------------------------------------------------------------------------
	//  Name : set_lods ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lods(const std::vector<asset_handle<mesh>>& lods);

	//-----------------------------------------------------------------------------
	//  Name : get_materials ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<asset_handle<material>>& get_materials() const;

	//-----------------------------------------------------------------------------
	//  Name : set_materials ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_materials(const std::vector<asset_handle<material>>& materials);

	//-----------------------------------------------------------------------------
	//  Name : get_material_for_group ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	asset_handle<material> get_material_for_group(const size_t& group) const;

	//-----------------------------------------------------------------------------
	//  Name : get_lod_transition_time ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_transition_time() const
	{
		return _transition_time;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_lod_max_distance ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_max_distance() const
	{
		return _max_distance;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_lod_min_distance ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_min_distance() const
	{
		return _min_distance;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_lod_transition_time ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_lod_transition_time(float time)
	{
		_transition_time = time;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_lod_max_distance ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lod_max_distance(float distance);

	//-----------------------------------------------------------------------------
	//  Name : set_lod_min_distance ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lod_min_distance(float distance);

	//-----------------------------------------------------------------------------
	//  Name : render ()
	/// <summary>
	/// Draws a mesh with a given program. If program is nullptr then the
	/// materials are used instead. Extra states can be added to the material
	/// ones.
	/// </summary>
	//-----------------------------------------------------------------------------
	void render(std::uint8_t id, const math::transform& mtx, bool apply_cull, bool depth_write,
				bool depth_test, std::uint64_t extra_states, unsigned int lod, program* user_program,
				std::function<void(program&)> setup_params) const;

private:
	/// Collection of all materials for this model.
	std::vector<asset_handle<material>> _materials;
	/// Default material
	asset_handle<material> _default_material;
	/// Collection of all lods for this model.
	std::vector<asset_handle<mesh>> _mesh_lods;
	/// Duration for a transition between two lods.
	float _transition_time = 1.0f;
	/// Maximum distance at which lod should have reached maximum value
	float _max_distance = 13.0;
	/// Maximum distance at which lod should have reached 0 (first lod)
	float _min_distance = 5.0f;
};
