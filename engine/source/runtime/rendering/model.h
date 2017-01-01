#pragma once

#include "../assets/asset_handle.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

#include <vector>

struct Group;
struct Mesh;
class Material;
class Model
{
public:
	REFLECTABLE(Model)
	SERIALIZABLE(Model)

	//-----------------------------------------------------------------------------
	//  Name : Model ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Model();

	//-----------------------------------------------------------------------------
	//  Name : ~Model (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~Model() = default;

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
	AssetHandle<Mesh> get_lod(std::uint32_t lod) const;

	//-----------------------------------------------------------------------------
	//  Name : set_lod ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lod(AssetHandle<Mesh> mesh, std::uint32_t lod);

	//-----------------------------------------------------------------------------
	//  Name : set_material ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_material(AssetHandle<Material> material, std::uint32_t index);

	//-----------------------------------------------------------------------------
	//  Name : get_lods ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<AssetHandle<Mesh>>& get_lods() const;

	//-----------------------------------------------------------------------------
	//  Name : set_lods ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_lods(const std::vector<AssetHandle<Mesh>>& lods);

	//-----------------------------------------------------------------------------
	//  Name : get_materials ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<AssetHandle<Material>>& get_materials() const;

	//-----------------------------------------------------------------------------
	//  Name : set_materials ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_materials(const std::vector<AssetHandle<Material>>& materials);

	//-----------------------------------------------------------------------------
	//  Name : get_material_for_group ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle<Material> get_material_for_group(const Group& group) const;

	//-----------------------------------------------------------------------------
	//  Name : get_lod_transition_time ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_transition_time() const { return _transition_time; }

	//-----------------------------------------------------------------------------
	//  Name : get_lod_max_distance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_max_distance() const { return _max_distance; }

	//-----------------------------------------------------------------------------
	//  Name : get_lod_min_distance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_lod_min_distance() const { return _min_distance; }

	//-----------------------------------------------------------------------------
	//  Name : set_lod_transition_time ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_lod_transition_time(float time) { _transition_time = time; }

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

private:
	/// Collection of all materials for this model.
	std::vector<AssetHandle<Material>> _materials;
	/// Collection of all lods for this model.
	std::vector<AssetHandle<Mesh>> _mesh_lods;
	/// Duration for a transition between two lods.
	float _transition_time = 1.0f;
	/// Maximum distance at which lod should have reached maximum value
	float _max_distance = 13.0;
	/// Maximum distance at which lod should have reached 0 (first lod)
	float _min_distance = 5.0f;
};