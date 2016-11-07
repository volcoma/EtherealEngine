#pragma once

#include "../Assets/AssetHandle.h"
#include "Core/math/math_includes.h"
#include <vector>

#include "Core/reflection/rttr/rttr_enable.h"
#include "Core/serialization/serialization.h"

struct Program;
struct Texture;

enum class CullType : std::uint32_t
{
	None,
	ClockWise,
	CounterClockWise,
};

class Material
{
public:
	REFLECTABLE(Material)
	SERIALIZABLE(Material)

	//-----------------------------------------------------------------------------
	//  Name : Material ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Material();

	//-----------------------------------------------------------------------------
	//  Name : ~Material (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~Material();

	//-----------------------------------------------------------------------------
	//  Name : isValid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool isValid() const { return !!mProgram; }

	//-----------------------------------------------------------------------------
	//  Name : getProgram ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Program* getProgram() const { return mProgram.get(); }

	//-----------------------------------------------------------------------------
	//  Name : submit (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void submit() {};

	//-----------------------------------------------------------------------------
	//  Name : getCullType ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline CullType getCullType() const { return mCullType; }

	//-----------------------------------------------------------------------------
	//  Name : setCullType ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setCullType(CullType val) { mCullType = val; }

	//-----------------------------------------------------------------------------
	//  Name : getRenderStates ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint64_t getRenderStates(bool applyCull = true, bool depthWrite = true, bool depthTest = true) const;
protected:
	/// Program that is responsible for rendering.
	std::unique_ptr<Program> mProgram;
	/// Cull type for this material.
	CullType mCullType = CullType::CounterClockWise;
	/// Default abledo texture
	AssetHandle<Texture> mDefaultAlbedo;
	/// Default normal texture
	AssetHandle<Texture> mDefaultNormal;
};


class StandardMaterial : public Material
{
public:
	REFLECTABLE(StandardMaterial, Material)
		SERIALIZABLE(StandardMaterial)

		StandardMaterial();
	const math::color& getBaseColor() const { return mBaseColor; }
	void setBaseColor(const math::color& val) { mBaseColor = val; }
	const math::color& getSpecularColor() const { return mSpecularColor; }
	void setSpecularColor(const math::color& val) { mSpecularColor = val; }
	const math::color& getEmissiveColor() const { return mEmissiveColor; }
	void setEmissiveColor(const math::color& val) { mEmissiveColor = val; }
	float getRoughness() const { return mSurfaceData.x; }
	void setRoughness(float rougness) { mSurfaceData.x = rougness; }
	float getMetalness() const { return mSurfaceData.y; }
	void setMetalness(float metalness) { mSurfaceData.y = metalness; }
	float getBumpiness() const { return mSurfaceData.z; }
	void setBumpiness(float bumpiness) { mSurfaceData.z = bumpiness; }
	float getAlphaTestValue() const { return mSurfaceData.w; }
	void setAlphaTestValue(float alphaTestValue) { mSurfaceData.w = alphaTestValue; }
	const math::vec4& getTiling() const { return mTiling; }
	void setTiling(const math::vec4& tiling) { mTiling = tiling; }
	const math::vec2& getDitherThreshold() const { return mDitherThreshold; }
	void setDitherThreshold(const math::vec2& threshold) { mDitherThreshold = threshold; }

	AssetHandle<Texture> getAlbedoTexture() const { return mAlbedo; }
	void setAlbedoTexture(AssetHandle<Texture> val) { mAlbedo = val; }
	AssetHandle<Texture> getNormalTexture() const { return mNormal; }
	void setNormalTexture(AssetHandle<Texture> val) { mNormal = val; }
	AssetHandle<Texture> getRoughnessTexture() const { return mRoughness; }
	void setRoughnessTexture(AssetHandle<Texture> val) { mRoughness = val; }
	AssetHandle<Texture> getMetalnessTexture() const { return mMetalness; }
	void setMetalnessTexture(AssetHandle<Texture> val) { mMetalness = val; }

	virtual void submit();
private:
	math::color mBaseColor
	{
		1.0f, 1.0f, 1.0f, // Color
		1.0f // Opacity
	};
	math::color mSpecularColor
	{
		0.5f, 0.5f, 0.5f, // Color
		1.0f // Unused
	};
	math::color mEmissiveColor
	{
		0.0f, 0.0f, 0.0f, // Color
		0.0f // HDR Scale
	};
	math::vec4 mSurfaceData
	{
		0.5f, // Roughness
		0.0f, // Metalness
		1.0f, // Bumpiness
		0.25f  // AlphaTestValue
	};
	math::vec4 mTiling
	{
		1.0f, 1.0f, //Primary
		1.0f, 1.0f	//Secondary
	};
	math::vec2 mDitherThreshold
	{
		0.5f, //Alpha threshold
		0.5f  //Distance threshold
	};

	AssetHandle<Texture> mAlbedo;
	AssetHandle<Texture> mNormal;
	AssetHandle<Texture> mRoughness;
	AssetHandle<Texture> mMetalness;
};