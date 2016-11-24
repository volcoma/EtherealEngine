#pragma once

#include "../Assets/AssetHandle.h"
#include "Core/math/math_includes.h"
#include <vector>

#include "Core/reflection/rttr/rttr_enable.h"
#include "Core/serialization/serialization.h"
#include "Graphics/graphics.h"

struct Program;
struct Texture;
struct FrameBuffer;

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
	//  Name : setTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setTexture(std::uint8_t _stage
		, const std::string& _sampler
		, FrameBuffer* _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : setTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setTexture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::FrameBufferHandle _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : setTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setTexture(std::uint8_t _stage
		, const std::string& _sampler
		, Texture* _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : setTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setTexture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::TextureHandle _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : setUniform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setUniform(const std::string& _name, const void* _value, std::uint16_t _num = 1);

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

	//-----------------------------------------------------------------------------
	//  Name : beginPass ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void beginPass();
protected:
	/// Program that is responsible for rendering.
	std::unique_ptr<Program> mProgram;
	/// Cull type for this material.
	CullType mCullType = CullType::CounterClockWise;
	/// Default color texture
	AssetHandle<Texture> mDefaultColorMap;
	/// Default normal texture
	AssetHandle<Texture> mDefaultNormalMap;
};


class StandardMaterial : public Material
{
public:
	REFLECTABLE(StandardMaterial, Material)
	SERIALIZABLE(StandardMaterial)

	//-----------------------------------------------------------------------------
	//  Name : StandardMaterial ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	StandardMaterial();

	//-----------------------------------------------------------------------------
	//  Name : getBaseColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& getBaseColor() const { return mBaseColor; }

	//-----------------------------------------------------------------------------
	//  Name : setBaseColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setBaseColor(const math::color& val) { mBaseColor = val; }

	//-----------------------------------------------------------------------------
	//  Name : getSpecularColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& getSpecularColor() const { return mSpecularColor; }

	//-----------------------------------------------------------------------------
	//  Name : setSpecularColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setSpecularColor(const math::color& val) { mSpecularColor = val; }

	//-----------------------------------------------------------------------------
	//  Name : getEmissiveColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& getEmissiveColor() const { return mEmissiveColor; }

	//-----------------------------------------------------------------------------
	//  Name : setEmissiveColor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setEmissiveColor(const math::color& val) { mEmissiveColor = val; }

	//-----------------------------------------------------------------------------
	//  Name : getRoughness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getRoughness() const { return mSurfaceData.x; }

	//-----------------------------------------------------------------------------
	//  Name : setRoughness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setRoughness(float rougness) { mSurfaceData.x = rougness; }

	//-----------------------------------------------------------------------------
	//  Name : getMetalness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getMetalness() const { return mSurfaceData.y; }

	//-----------------------------------------------------------------------------
	//  Name : setMetalness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setMetalness(float metalness) { mSurfaceData.y = metalness; }

	//-----------------------------------------------------------------------------
	//  Name : getBumpiness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getBumpiness() const { return mSurfaceData.z; }

	//-----------------------------------------------------------------------------
	//  Name : setBumpiness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setBumpiness(float bumpiness) { mSurfaceData.z = bumpiness; }

	//-----------------------------------------------------------------------------
	//  Name : getAlphaTestValue ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getAlphaTestValue() const { return mSurfaceData.w; }

	//-----------------------------------------------------------------------------
	//  Name : setAlphaTestValue ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setAlphaTestValue(float alphaTestValue) { mSurfaceData.w = alphaTestValue; }

	//-----------------------------------------------------------------------------
	//  Name : getTiling ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec4& getTiling() const { return mTiling; }

	//-----------------------------------------------------------------------------
	//  Name : setTiling ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setTiling(const math::vec4& tiling) { mTiling = tiling; }

	//-----------------------------------------------------------------------------
	//  Name : getDitherThreshold ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec2& getDitherThreshold() const { return mDitherThreshold; }

	//-----------------------------------------------------------------------------
	//  Name : setDitherThreshold ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setDitherThreshold(const math::vec2& threshold) { mDitherThreshold = threshold; }

	//-----------------------------------------------------------------------------
	//  Name : getAlbedoTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> getColorMap() const { return mColorMap; }

	//-----------------------------------------------------------------------------
	//  Name : setAlbedoTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setColorMap(AssetHandle<Texture> val) { mColorMap = val; }

	//-----------------------------------------------------------------------------
	//  Name : getNormalTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> getNormalMap() const { return mNormalMap; }

	//-----------------------------------------------------------------------------
	//  Name : setNormalTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setNormalMap(AssetHandle<Texture> val) { mNormalMap = val; }

	//-----------------------------------------------------------------------------
	//  Name : getRoughnessTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> getRoughnessMap() const { return mRoughnessMap; }

	//-----------------------------------------------------------------------------
	//  Name : setRoughnessTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setRoughnessMap(AssetHandle<Texture> val) { mRoughnessMap = val; }

	//-----------------------------------------------------------------------------
	//  Name : getMetalnessTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> getMetalnessMap() const { return mMetalnessMap; }

	//-----------------------------------------------------------------------------
	//  Name : setMetalnessTexture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setMetalnessMap(AssetHandle<Texture> val) { mMetalnessMap = val; }

	//-----------------------------------------------------------------------------
	//  Name : submit (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void submit();
private:
	/// Base color
	math::color mBaseColor
	{
		1.0f, 1.0f, 1.0f, /// Color
		1.0f /// Opacity
	};
	/// Specular color
	math::color mSpecularColor
	{
		0.5f, 0.5f, 0.5f, /// Color
		1.0f /// Unused
	};
	/// Emissive color
	math::color mEmissiveColor
	{
		0.0f, 0.0f, 0.0f, /// Color
		0.0f /// HDR Scale
	};
	/// Surface data
	math::vec4 mSurfaceData
	{
		0.5f, /// Roughness
		0.0f, /// Metalness
		1.0f, /// Bumpiness
		0.25f /// AlphaTestValue
	};
	/// Tiling data
	math::vec4 mTiling
	{
		1.0f, 1.0f, ///Primary
		1.0f, 1.0f  ///Secondary
	};
	/// Dithering data
	math::vec2 mDitherThreshold
	{
		0.5f, ///Alpha threshold
		0.5f  ///Distance threshold
	};
	/// Color map
	AssetHandle<Texture> mColorMap;
	/// Normal map
	AssetHandle<Texture> mNormalMap;
	/// Roughness map
	AssetHandle<Texture> mRoughnessMap;
	/// Metalness map
	AssetHandle<Texture> mMetalnessMap;
};