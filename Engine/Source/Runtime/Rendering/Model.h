#pragma once

#include "../Assets/AssetHandle.h"
#include "Core/reflection/rttr/rttr_enable.h"
#include "Core/serialization/serialization.h"

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
	//  Name : isValid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isValid() const;

	//-----------------------------------------------------------------------------
	//  Name : getLod ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle<Mesh> getLod(std::uint32_t lod) const;

	//-----------------------------------------------------------------------------
	//  Name : setLod ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setLod(AssetHandle<Mesh> mesh, std::uint32_t lod);

	//-----------------------------------------------------------------------------
	//  Name : setMaterial ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setMaterial(AssetHandle<Material> material, std::uint32_t index);

	//-----------------------------------------------------------------------------
	//  Name : getLods ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<AssetHandle<Mesh>>& getLods() const;

	//-----------------------------------------------------------------------------
	//  Name : setLods ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setLods(const std::vector<AssetHandle<Mesh>>& lods);

	//-----------------------------------------------------------------------------
	//  Name : getMaterials ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<AssetHandle<Material>>& getMaterials() const;

	//-----------------------------------------------------------------------------
	//  Name : setMaterials ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setMaterials(const std::vector<AssetHandle<Material>>& materials);

	//-----------------------------------------------------------------------------
	//  Name : getMaterialForGroup ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle<Material> getMaterialForGroup(const Group& group) const;

	//-----------------------------------------------------------------------------
	//  Name : getTransitionTime ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getTransitionTime() const { return mTransitionTime; }

	//-----------------------------------------------------------------------------
	//  Name : getMaxDistance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getMaxDistance() const { return mMaxDistance; }

	//-----------------------------------------------------------------------------
	//  Name : getMinDistance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getMinDistance() const { return mMinDistance; }

	//-----------------------------------------------------------------------------
	//  Name : setTransitionTime ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setTransitionTime(float time) { mTransitionTime = time; }

	//-----------------------------------------------------------------------------
	//  Name : setMaxDistance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setMaxDistance(float distance);

	//-----------------------------------------------------------------------------
	//  Name : setMinDistance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setMinDistance(float distance);

private:
	/// Collection of all materials for this model.
	std::vector<AssetHandle<Material>> mMaterials;
	/// Collection of all lods for this model.
	std::vector<AssetHandle<Mesh>> mMeshLods;
	/// Duration for a transition between two lods.
	float mTransitionTime = 1.0f;
	/// Maximum distance at which lod should have reached maximum value
	float mMaxDistance = 13.0;
	/// Maximum distance at which lod should have reached 0 (first lod)
	float mMinDistance = 5.0f;
};