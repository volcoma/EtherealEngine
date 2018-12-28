#pragma once

#include "../../rendering/gpu_program.h"
#include "../components/model_component.h"
#include "../components/transform_component.h"
#include "../ecs.h"

#include <core/common/basetypes.hpp>

#include <chrono>
#include <memory>
#include <tuple>
#include <vector>

class camera;

namespace gfx
{
struct texture;
struct frame_buffer;
class render_view;
}

namespace runtime
{
struct lod_data
{
	std::uint32_t current_lod_index = 0;
	std::uint32_t target_lod_index = 0;
	float current_time = 0.0f;
};

using visibility_set_models_t =
	std::vector<std::tuple<entity, chandle<transform_component>, chandle<model_component>>>;

class deferred_rendering
{
public:
	deferred_rendering();
	~deferred_rendering();
	//-----------------------------------------------------------------------------
	//  Name : gather_visible_models ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	visibility_set_models_t gather_visible_models(entity_component_system& ecs, camera* camera,
												  bool dirty_only = false, bool static_only = true,
												  bool require_reflection_caster = false);
	//-----------------------------------------------------------------------------
	//  Name : frame_render (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_render(delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : receive ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void receive(entity e);

	//-----------------------------------------------------------------------------
	//  Name : build_reflections ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void build_reflections_pass(entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : build_shadows ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void build_shadows_pass(entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : camera_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void camera_pass(entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : scene_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> deferred_render_full(camera& camera, gfx::render_view& render_view,
															entity_component_system& ecs,
															std::unordered_map<entity, lod_data>& camera_lods,
															delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : g_buffer_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> g_buffer_pass(std::shared_ptr<gfx::frame_buffer> input, camera& camera,
													 gfx::render_view& render_view,
													 visibility_set_models_t& visibility_set,
													 std::unordered_map<entity, lod_data>& camera_lods,
													 delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : lighting_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> lighting_pass(std::shared_ptr<gfx::frame_buffer> input, camera& camera,
													 gfx::render_view& render_view,
													 entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : reflection_probe ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> reflection_probe_pass(std::shared_ptr<gfx::frame_buffer> input,
															 camera& camera, gfx::render_view& render_view,
															 entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : atmospherics_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> atmospherics_pass(std::shared_ptr<gfx::frame_buffer> input,
														 camera& camera, gfx::render_view& render_view,
														 entity_component_system& ecs, delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : tonemapping_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> tonemapping_pass(std::shared_ptr<gfx::frame_buffer> input,
														camera& camera, gfx::render_view& render_view);

private:
	std::unordered_map<entity, std::unordered_map<entity, lod_data>> lod_data_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> directional_light_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> point_light_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> spot_light_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> box_ref_probe_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> sphere_ref_probe_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> gamma_correction_program_;
	/// Program that is responsible for rendering.
	std::unique_ptr<gpu_program> atmospherics_program_;
	///
	asset_handle<gfx::texture> ibl_brdf_lut_;
};
}
