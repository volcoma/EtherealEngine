#pragma once

#include "../assets/asset_handle.h"
#include "core/math/math_includes.h"
#include <vector>

#include "core/reflection/rttr/rttr_enable.h"
#include "core/serialization/serialization.h"
#include "graphics/graphics.h"

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
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_valid() const { return !!_program; }

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, FrameBuffer* _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::FrameBufferHandle _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, Texture* _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::TextureHandle _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_uniform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_uniform(const std::string& _name, const void* _value, std::uint16_t _num = 1);

	//-----------------------------------------------------------------------------
	//  Name : get_program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Program* get_program() const { return _program.get(); }

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
	//  Name : get_cull_type ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline CullType get_cull_type() const { return _cull_type; }

	//-----------------------------------------------------------------------------
	//  Name : set_cull_type ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_cull_type(CullType val) { _cull_type = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_render_states ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint64_t get_render_states(bool applyCull = true, bool depthWrite = true, bool depthTest = true) const;

	//-----------------------------------------------------------------------------
	//  Name : begin_pass ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void begin_pass();
protected:
	/// Program that is responsible for rendering.
	std::unique_ptr<Program> _program;
	/// Cull type for this material.
	CullType _cull_type = CullType::CounterClockWise;
	/// Default color texture
	AssetHandle<Texture> _default_color_map;
	/// Default normal texture
	AssetHandle<Texture> _default_normal_map;
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
	inline const math::color& get_base_color() const { return _base_color; }

	//-----------------------------------------------------------------------------
	//  Name : set_base_color ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_base_color(const math::color& val) { _base_color = val; }

	//-----------------------------------------------------------------------------
	//  Name : set_specular_color ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& get_specular_color() const { return _specular_color; }

	//-----------------------------------------------------------------------------
	//  Name : set_specular_color ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_specular_color(const math::color& val) { _specular_color = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_emissive_color ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& get_emissive_color() const { return _emissive_color; }

	//-----------------------------------------------------------------------------
	//  Name : set_emissive_color ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_emissive_color(const math::color& val) { _emissive_color = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_roughness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_roughness() const { return _surface_data.x; }

	//-----------------------------------------------------------------------------
	//  Name : set_roughness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_roughness(float rougness) { _surface_data.x = rougness; }

	//-----------------------------------------------------------------------------
	//  Name : get_metalness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_metalness() const { return _surface_data.y; }

	//-----------------------------------------------------------------------------
	//  Name : set_metalness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_metalness(float metalness) { _surface_data.y = metalness; }

	//-----------------------------------------------------------------------------
	//  Name : get_bumpiness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_bumpiness() const { return _surface_data.z; }

	//-----------------------------------------------------------------------------
	//  Name : set_bumpiness ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_bumpiness(float bumpiness) { _surface_data.z = bumpiness; }

	//-----------------------------------------------------------------------------
	//  Name : get_alpha_test_value ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_alpha_test_value() const { return _surface_data.w; }

	//-----------------------------------------------------------------------------
	//  Name : set_alpha_test_value ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_alpha_test_value(float alphaTestValue) { _surface_data.w = alphaTestValue; }

	//-----------------------------------------------------------------------------
	//  Name : set_tiling ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec4& get_tiling() const { return _tiling; }

	//-----------------------------------------------------------------------------
	//  Name : set_tiling ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_tiling(const math::vec4& tiling) { _tiling = tiling; }

	//-----------------------------------------------------------------------------
	//  Name : set_dither_threshold ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec2& get_dither_threshold() const { return _dither_threshold; }

	//-----------------------------------------------------------------------------
	//  Name : set_dither_threshold ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_dither_threshold(const math::vec2& threshold) { _dither_threshold = threshold; }

	//-----------------------------------------------------------------------------
	//  Name : set_color_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> get_color_map() const { return _color_map; }

	//-----------------------------------------------------------------------------
	//  Name : set_color_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_color_map(AssetHandle<Texture> val) { _color_map = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_normal_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> get_normal_map() const { return _normal_map; }

	//-----------------------------------------------------------------------------
	//  Name : set_normal_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_normal_map(AssetHandle<Texture> val) { _normal_map = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_roughness_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> get_roughness_map() const { return _roughness_map; }

	//-----------------------------------------------------------------------------
	//  Name : set_roughness_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_roughness_map(AssetHandle<Texture> val) { _roughness_map = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_metalness_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetHandle<Texture> get_metalness_map() const { return _metalness_map; }

	//-----------------------------------------------------------------------------
	//  Name : set_metalness_map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_metalness_map(AssetHandle<Texture> val) { _metalness_map = val; }

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
	math::color _base_color
	{
		1.0f, 1.0f, 1.0f, /// Color
		1.0f /// Opacity
	};
	/// Specular color
	math::color _specular_color
	{
		0.5f, 0.5f, 0.5f, /// Color
		1.0f /// Unused
	};
	/// Emissive color
	math::color _emissive_color
	{
		0.0f, 0.0f, 0.0f, /// Color
		0.0f /// HDR Scale
	};
	/// Surface data
	math::vec4 _surface_data
	{
		0.5f, /// Roughness
		0.0f, /// Metalness
		1.0f, /// Bumpiness
		0.25f /// AlphaTestValue
	};
	/// Tiling data
	math::vec4 _tiling
	{
		1.0f, 1.0f, ///Primary
		1.0f, 1.0f  ///Secondary
	};
	/// Dithering data
	math::vec2 _dither_threshold
	{
		0.5f, ///Alpha threshold
		0.5f  ///Distance threshold
	};
	/// Color map
	AssetHandle<Texture> _color_map;
	/// Normal map
	AssetHandle<Texture> _normal_map;
	/// Roughness map
	AssetHandle<Texture> _roughness_map;
	/// Metalness map
	AssetHandle<Texture> _metalness_map;
};