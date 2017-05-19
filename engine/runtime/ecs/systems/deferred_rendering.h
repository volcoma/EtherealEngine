#pragma once

#include "../ecs.h"
#include <vector>
#include <memory>
#include <chrono>
#include <tuple>
#include "../../rendering/program.h"
#include "../components/transform_component.h"
#include "../components/model_component.h"

class camera;
class render_view;


namespace runtime
{
	struct lod_data
	{
		std::uint32_t current_lod_index = 0;
		std::uint32_t target_lod_index = 0;
		float current_time = 0.0f;
	};

	using visibility_set_models_t = std::vector<std::tuple<entity, chandle<transform_component>, chandle<model_component>>>;

	class deferred_rendering : public core::subsystem
	{
	public:
		//-----------------------------------------------------------------------------
		//  Name : gather_visible_models ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		visibility_set_models_t gather_visible_models(entity_component_system& ecs, camera* camera, bool dirty_only = false, bool static_only = true, bool require_reflection_caster = false);
		//-----------------------------------------------------------------------------
		//  Name : frame_render (virtual )
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void frame_render(std::chrono::duration<float> dt);

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
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize() override;

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose() override;

		//-----------------------------------------------------------------------------
		//  Name : build_reflections ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void build_reflections_pass(entity_component_system& ecs, std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : build_shadows ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void build_shadows_pass(entity_component_system& ecs, std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : camera_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void camera_pass(entity_component_system& ecs, std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : scene_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> deferred_render_full(
			camera& camera,
			render_view& render_view,
			entity_component_system& ecs,
			std::unordered_map<entity, lod_data>& camera_lods,
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : g_buffer_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> g_buffer_pass(
			std::shared_ptr<frame_buffer> input,
			camera& camera,
			render_view& render_view,
			visibility_set_models_t& visibility_set,
			std::unordered_map<entity, lod_data>& camera_lods, 
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : lighting_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> lighting_pass(
			std::shared_ptr<frame_buffer> input,
			camera& camera, 
			render_view& render_view,
			entity_component_system& ecs,
			std::chrono::duration<float> dt,
			bool bind_indirect_specular);

		//-----------------------------------------------------------------------------
		//  Name : reflection_probe ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> reflection_probe_pass(
			std::shared_ptr<frame_buffer> input,
			camera& camera,
			render_view& render_view,
			entity_component_system& ecs,
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : atmospherics_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> atmospherics_pass(
			std::shared_ptr<frame_buffer> input,
			camera& camera,
			render_view& render_view,
			entity_component_system& ecs,
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : tonemapping_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<frame_buffer> tonemapping_pass(
			std::shared_ptr<frame_buffer> input,
			camera& camera,
			render_view& render_view);
	private:
		std::unordered_map<entity, std::unordered_map<entity, lod_data>> _lod_data;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _directional_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _point_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _spot_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _box_ref_probe_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _sphere_ref_probe_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _gamma_correction_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<program> _atmospherics_program;
	};

}