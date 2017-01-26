#pragma once

#include "../ecs.h"
#include <vector>
#include <memory>
#include <chrono>
#include "../../rendering/program.h"

class CameraComponent;

namespace runtime
{
	struct LodData
	{
		std::uint32_t current_lod_index = 0;
		std::uint32_t target_lod_index = 0;
		float current_time = 0.0f;
	};

	class DeferredRendering : public core::Subsystem
	{
	public:
		//-----------------------------------------------------------------------------
		//  Name : frameRender (virtual )
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
		void receive(Entity e);

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
		//  Name : g_buffer_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<FrameBuffer> g_buffer_pass(
			std::shared_ptr<FrameBuffer> input,
			CameraComponent& camera_comp,
			std::unordered_map<Entity,
			LodData>& camera_lods, 
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : lighting_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<FrameBuffer> lighting_pass(
			std::shared_ptr<FrameBuffer> input,
			CameraComponent& camera_comp, 
			std::chrono::duration<float> dt);

		//-----------------------------------------------------------------------------
		//  Name : tonemapping_pass ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		std::shared_ptr<FrameBuffer> tonemapping_pass(
			std::shared_ptr<FrameBuffer> input,
			CameraComponent& camera_comp);
	private:
		std::unordered_map<Entity, std::unordered_map<Entity, LodData>> _lod_data;
		/// Program that is responsible for rendering.
		std::unique_ptr<Program> _directional_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<Program> _point_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<Program> _spot_light_program;
		/// Program that is responsible for rendering.
		std::unique_ptr<Program> _gamma_correction_program;
	};

}