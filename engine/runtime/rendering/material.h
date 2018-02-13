#pragma once

#include "../assets/asset_handle.h"
#include "core/graphics/graphics.h"
#include "core/math/math_includes.h"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"
#include "core/tasks/task_system.h"
#include <unordered_map>

class gpu_program;
namespace gfx
{
struct texture;
struct frame_buffer;
}

enum class cull_type : std::uint32_t
{
	none,
	clockwise,
	counter_clockwise,
};

class material
{
public:
	REFLECTABLEV(material)
	SERIALIZABLE(material)

	//-----------------------------------------------------------------------------
	//  Name : material ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	material();

	//-----------------------------------------------------------------------------
	//  Name : ~material (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~material();

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_valid() const
	{
		return !(get_program() == nullptr);
	}

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::frame_buffer* _handle,
					 uint8_t _attachment = 0,
					 std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::texture* _texture,
					 std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

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
	gpu_program* get_program() const;

	//-----------------------------------------------------------------------------
	//  Name : submit (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void submit()
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : get_cull_type ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline cull_type get_cull_type() const
	{
		return cull_type_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_cull_type ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_cull_type(cull_type val)
	{
		cull_type_ = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_render_states ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint64_t get_render_states(bool apply_cull = true, bool depth_write = true,
									bool depth_test = true) const;

	bool skinned = false;

protected:
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> program_skinned_;
	/// Cull type for this material.
	cull_type cull_type_ = cull_type::counter_clockwise;
	/// Default color texture
	asset_handle<gfx::texture> default_color_map_;
	/// Default normal texture
	asset_handle<gfx::texture> default_normal_map_;

	std::vector<core::task_future<void>> futures_;
};

class standard_material : public material
{
public:
	SERIALIZABLE(standard_material)
	REFLECTABLEV(standard_material, material)

	//-----------------------------------------------------------------------------
	//  Name : standard_material ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	standard_material();
	~standard_material();
	//-----------------------------------------------------------------------------
	//  Name : get_base_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& get_base_color() const
	{
		return base_color_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_base_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_base_color(const math::color& val)
	{
		base_color_ = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_subsurface_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& get_subsurface_color() const
	{
		return subsurface_color_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_subsurface_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_subsurface_color(const math::color& val)
	{
		subsurface_color_ = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_emissive_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::color& get_emissive_color() const
	{
		return emissive_color_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_emissive_color ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_emissive_color(const math::color& val)
	{
		emissive_color_ = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_roughness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_roughness() const
	{
		return surface_data_.x;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_roughness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_roughness(float rougness)
	{
		surface_data_.x = rougness;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_metalness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_metalness() const
	{
		return surface_data_.y;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_metalness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_metalness(float metalness)
	{
		surface_data_.y = metalness;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_bumpiness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_bumpiness() const
	{
		return surface_data_.z;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_bumpiness ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_bumpiness(float bumpiness)
	{
		surface_data_.z = bumpiness;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_alpha_test_value ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_alpha_test_value() const
	{
		return surface_data_.w;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_alpha_test_value ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_alpha_test_value(float alphaTestValue)
	{
		surface_data_.w = alphaTestValue;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_tiling ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec2& get_tiling() const
	{
		return tiling_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_tiling ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_tiling(const math::vec2& tiling)
	{
		tiling_ = tiling;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_dither_threshold ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec2& get_dither_threshold() const
	{
		return dither_threshold_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_dither_threshold ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_dither_threshold(const math::vec2& threshold)
	{
		dither_threshold_ = threshold;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_color_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline asset_handle<gfx::texture> get_color_map()
	{
		return maps_["color"];
	}

	//-----------------------------------------------------------------------------
	//  Name : set_color_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_color_map(asset_handle<gfx::texture> val)
	{
		maps_["color"] = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_normal_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline asset_handle<gfx::texture> get_normal_map()
	{
		return maps_["normal"];
	}

	//-----------------------------------------------------------------------------
	//  Name : set_normal_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_normal_map(asset_handle<gfx::texture> val)
	{
		maps_["normal"] = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_roughness_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline asset_handle<gfx::texture> get_roughness_map()
	{
		return maps_["roughness"];
	}

	//-----------------------------------------------------------------------------
	//  Name : set_roughness_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_roughness_map(asset_handle<gfx::texture> val)
	{
		maps_["roughness"] = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_metalness_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline asset_handle<gfx::texture> get_metalness_map()
	{
		return maps_["metalness"];
	}

	//-----------------------------------------------------------------------------
	//  Name : set_metalness_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_metalness_map(asset_handle<gfx::texture> val)
	{
		maps_["metalness"] = val;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_ao_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline asset_handle<gfx::texture> get_ao_map()
	{
		return maps_["ao"];
	}

	//-----------------------------------------------------------------------------
	//  Name : set_ao_map ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_ao_map(asset_handle<gfx::texture> val)
	{
		maps_["ao"] = val;
	}

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
	math::color base_color_{
		1.0f, 1.0f, 1.0f, /// Color
		1.0f			  /// Opacity
	};
	/// Emissive color
	math::color subsurface_color_{
		0.0f, 0.0f, 0.0f, /// Color
		0.8f			  /// Opacity
	};
	/// Emissive color
	math::color emissive_color_{
		0.0f, 0.0f, 0.0f, /// Color
		0.0f			  /// HDR Scale
	};
	/// Surface data
	math::vec4 surface_data_{
		0.3f, /// Roughness
		0.0f, /// Metalness
		1.0f, /// Bumpiness
		0.25f /// AlphaTestValue
	};
	/// Tiling data
	math::vec2 tiling_{
		1.0f, 1.0f /// Primary
	};
	/// Dithering data
	math::vec2 dither_threshold_{
		0.5f, /// Alpha threshold
		0.0f  /// Distance threshold
	};

	/// Texture maps
	std::unordered_map<std::string, asset_handle<gfx::texture>> maps_;
};
