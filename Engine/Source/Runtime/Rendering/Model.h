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

	Model();
	virtual ~Model() {}
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
	void setMaterial(AssetHandle<Material> material, std::uint32_t index);
	const std::vector<AssetHandle<Mesh>>& getLods() const;
	void setLods(const std::vector<AssetHandle<Mesh>>& lods);
	const std::vector<AssetHandle<Material>>& getMaterials() const;
	void setMaterials(const std::vector<AssetHandle<Material>>& materials);

	AssetHandle<Material> getMaterialForGroup(const Group& group) const;

	float getTransitionTime() const { return mTransitionTime; }
	float getMaxDistance() const { return mMaxDistance; }
	float getMinDistance() const { return mMinDistance; }

	void setTransitionTime(float time) { mTransitionTime = time; }
	void setMaxDistance(float distance) 
	{ 
		if (distance < mMinDistance)
			distance = mMinDistance;

		mMaxDistance = distance; 
	}
	void setMinDistance(float distance) 
	{
		if (distance > mMaxDistance)
			distance = mMaxDistance;

		mMinDistance = distance;
	}

private:
	std::vector<AssetHandle<Mesh>> mMeshLods;
	std::vector<AssetHandle<Material>> mMaterials;
	float mTransitionTime = 1.0f;
	float mMaxDistance = 13.0;
	float mMinDistance = 5.0f;

};