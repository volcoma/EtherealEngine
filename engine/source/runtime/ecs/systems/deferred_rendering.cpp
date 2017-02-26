#include "deferred_rendering.h"
#include "../components/transform_component.h"
#include "../components/camera_component.h"
#include "../components/model_component.h"
#include "../components/light_component.h"
#include "../components/reflection_probe_component.h"
#include "../../rendering/render_pass.h"
#include "../../rendering/camera.h"
#include "../../rendering/mesh.h"
#include "../../rendering/model.h"
#include "../../rendering/vertex_buffer.h"
#include "../../rendering/index_buffer.h"
#include "../../rendering/texture.h"
#include "../../rendering/material.h"
#include "../../system/engine.h"
#include "../../assets/asset_manager.h"

namespace runtime
{
	Camera get_face_camera(std::uint32_t face,  const math::transform_t& transform)
	{
		Camera camera;
		camera.set_fov(90.0f);
		camera.set_aspect_ratio(1.0f, true);
		camera.set_near_clip(0.01f);
		camera.set_far_clip(256.0f);


		// Configurable axis vectors used to construct view matrices. In the 
		// case of the omni light, we align all frustums to the world axes.
		math::vec3 X(1, 0, 0);
		math::vec3 Y(0, 1, 0);
		math::vec3 Z(0, 0, 1);

		math::transform_t t;
		// Generate the correct view matrix for the frustum
		if (!gfx::is_origin_bottom_left())
		{
			switch (face)
			{
			case 1:
				t.look_at({}, +X, +Y); break;
			case 0:
				t.look_at({}, -X, +Y); break;
			case 3:
				t.look_at({}, +Y, -Z); break;
			case 2:
				t.look_at({}, -Y, +Z); break;
			case 4:
				t.look_at({}, +Z, +Y); break;
			case 5:
				t.look_at({}, -Z, +Y); break;
			}
		}
		else
		{
			switch (face)
			{
			case 1:
				t.look_at({}, +X, +Y); break;
			case 0:
				t.look_at({}, -X, +Y); break;
			case 2:
				t.look_at({}, +Y, -Z); break;
			case 3:
				t.look_at({}, -Y, +Z); break;
			case 4:
				t.look_at({}, +Z, +Y); break;
			case 5:
				t.look_at({}, -Z, +Y); break;
			}

		}
		
	
		t = transform * t;
		// First update so the camera can cache the previous matrices
		camera.record_current_matrices();
		// Set new transform
		camera.look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());

		return camera;
	}

	void update_lod_data(LodData& lod_data, std::size_t total_lods, float min_dist, float max_dist, float transition_time, float distance, float dt)
	{
		if (total_lods == 1)
			return;

		total_lods -= 1;
		if (total_lods < 0)
			total_lods = 0;

		const float factor = 1.0f - math::clamp((max_dist - distance) / (max_dist - min_dist), 0.0f, 1.0f);
		const int lod = (int)math::lerp(0.0f, static_cast<float>(total_lods), factor);
		if (lod_data.target_lod_index != lod && lod_data.target_lod_index == lod_data.current_lod_index)
			lod_data.target_lod_index = lod;

		if (lod_data.current_lod_index != lod_data.target_lod_index)
			lod_data.current_time += dt;

		if (lod_data.current_time >= transition_time)
		{
			lod_data.current_lod_index = lod_data.target_lod_index;
			lod_data.current_time = 0.0f;
		}
	}

	bool should_rebuild_reflections(VisibilitySetModels& visibility_set, const ReflectionProbe& probe)
	{

 		if (probe.method == ReflectMethod::Environment)
 			return false;

		for (auto& element : visibility_set)
		{
			auto& e = std::get<0>(element);
			auto& transform_comp_handle = std::get<1>(element);
			auto& model_comp_handle = std::get<2>(element);
			auto transform_comp_ptr = transform_comp_handle.lock();
			auto model_comp_ptr = model_comp_handle.lock();
			if (!transform_comp_ptr || !model_comp_ptr)
				continue;

			auto& transform_comp_ref = *transform_comp_ptr.get();
			auto& model_comp_ref = *model_comp_ptr.get();

			const auto& model = model_comp_ref.get_model();
			if (!model.is_valid())
				continue;

			const auto mesh = model.get_lod(0);

			const auto& world_transform = transform_comp_ref.get_transform();

			const auto& bounds = mesh->get_bounds();


			bool result = false;

			for (std::uint32_t i = 0; i < 6; ++i)
			{			
				const auto& frustum = get_face_camera(i, world_transform).get_frustum();
				result |= math::frustum::test_obb(frustum, bounds, world_transform);
			}

			if (result)
				return true;
		}

		return false;
	}

	VisibilitySetModels DeferredRendering::gather_visible_models(EntityComponentSystem& ecs, Camera* camera, bool dirty_only/* = false*/, bool static_only /*= true*/, bool require_reflection_caster /*= false*/)
	{
		VisibilitySetModels result;
		CHandle<TransformComponent> transform_comp_handle;
		CHandle<ModelComponent> model_comp_handle;
		for (auto entity : ecs.entities_with_components(transform_comp_handle, model_comp_handle))
		{
			auto model_comp_ptr = model_comp_handle.lock();
			auto transform_comp_ptr = transform_comp_handle.lock();

			if (static_only && !model_comp_ptr->is_static())
			{
				continue;
			}

			if (require_reflection_caster && !model_comp_ptr->casts_reflection())
			{
				continue;
			}

			auto mesh = model_comp_ptr->get_model().get_lod(0);

			// If mesh isnt loaded yet skip it.
			if (!mesh)
				continue;

			if (camera)
			{
				const auto& frustum = camera->get_frustum();

				const auto& world_transform = transform_comp_ptr->get_transform();

				const auto& bounds = mesh->get_bounds();

				// Test the bounding box of the mesh
				if (math::frustum::test_obb(frustum, bounds, world_transform))
				{
					// Only dirty Mesh components.
					if (dirty_only)
					{
						if (transform_comp_ptr->is_dirty() || model_comp_ptr->is_dirty())
						{
							result.push_back({ entity, transform_comp_handle, model_comp_handle });
						}
					} // End if dirty_only
					else
					{
						result.push_back({ entity, transform_comp_handle, model_comp_handle });
					}

				} // Enf if visble
			}
			else
			{
				// Only dirty Mesh components.
				if (dirty_only)
				{
					if (transform_comp_ptr->is_dirty() || model_comp_ptr->is_dirty())
					{
						result.push_back({ entity, transform_comp_handle, model_comp_handle });
					}
				} // End if dirty_only
				else
				{
					result.push_back({ entity, transform_comp_handle, model_comp_handle });
				}
			}

		}
		return result;
	}


	void DeferredRendering::frame_render(std::chrono::duration<float> dt)
	{
		auto& ecs = *core::get_subsystem<EntityComponentSystem>();

		//////////////////////////////////////////////////////////////////////////
		// Reflection Probe Generation
		//////////////////////////////////////////////////////////////////////////
		auto dirty_models = gather_visible_models(ecs, nullptr, true, true, true);
		ecs.each<TransformComponent, ReflectionProbeComponent>([this, &ecs, dt, &dirty_models](
			Entity ce,
			TransformComponent& transform_comp,
			ReflectionProbeComponent& reflection_probe_comp
			)
		{
			const auto& world_tranform = transform_comp.get_transform();
			const auto& probe = reflection_probe_comp.get_probe();
			auto& render_view = reflection_probe_comp.get_render_view();
			auto cubemap = reflection_probe_comp.get_cubemap();
			bool should_rebuild = true;

			if (!transform_comp.is_dirty() && !reflection_probe_comp.is_dirty())
			{
				// If reflections shouldn't be rebuilt - continue.
				should_rebuild = should_rebuild_reflections(dirty_models, probe);
			}

			if (!should_rebuild)
				return;

			//iterate trough each cube face
			for (std::uint32_t i = 0; i < 6; ++i)
			{
				auto camera = get_face_camera(i, world_tranform);
				camera.set_viewport_size(cubemap->get_size());
				auto& camera_lods = _lod_data[ce];
				VisibilitySetModels visibility_set;

				if(probe.method != ReflectMethod::Environment)
					visibility_set = gather_visible_models(ecs, &camera, !should_rebuild, true, true);

				std::shared_ptr<FrameBuffer> output = nullptr;		
				output = g_buffer_pass(output, camera, render_view, visibility_set, camera_lods, dt);	
				output = lighting_pass(output, camera, render_view, ecs, dt, false);	
				output = atmospherics_pass(output, camera, render_view, ecs, dt);	
				output = tonemapping_pass(output, camera, render_view);
		
				RenderPass pass("cubemap_fill");
				gfx::blit(pass.id, cubemap->handle, 0, 0, 0, i, gfx::getTexture(output->handle));
			}
		
			auto cubemap_fbo = render_view.get_fbo("CUBEMAP", { cubemap });
			RenderPass pass("cubemap_generate_mips");
			pass.bind(cubemap_fbo.get());
			
		});
		
		//////////////////////////////////////////////////////////////////////////
		// Camera Render
		//////////////////////////////////////////////////////////////////////////
		ecs.each<CameraComponent>([this, &ecs, dt](
			Entity ce,
			CameraComponent& camera_comp
			)
		{
			auto& camera_lods = _lod_data[ce];
			auto& camera = camera_comp.get_camera();
			auto& render_view = camera_comp.get_render_view();

			auto output = deferred_render_full(camera, render_view, ecs, camera_lods, dt);
		});
	}


	std::shared_ptr<FrameBuffer> DeferredRendering::deferred_render_full(
		Camera& camera,
		RenderView& render_view, 
		EntityComponentSystem& ecs, 
		std::unordered_map<Entity, LodData>& camera_lods, 
		std::chrono::duration<float> dt)
	{
		std::shared_ptr<FrameBuffer> output = nullptr;

		auto visibility_set = gather_visible_models(ecs, &camera, false, false, false);

		output = g_buffer_pass(output, camera, render_view, visibility_set, camera_lods, dt);

		output = reflection_probe_pass(output, camera, render_view, ecs, dt);

		output = lighting_pass(output, camera, render_view, ecs, dt, true);

		output = atmospherics_pass(output, camera, render_view, ecs, dt);

		output = tonemapping_pass(output, camera, render_view);

		return output;
	}

	std::shared_ptr<FrameBuffer> DeferredRendering::g_buffer_pass(
		std::shared_ptr<FrameBuffer> input,
		Camera& camera,
		RenderView& render_view,
		VisibilitySetModels& visibility_set,
		std::unordered_map<Entity, LodData>& camera_lods, 
		std::chrono::duration<float> dt)
	{
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		const auto& viewport_size = camera.get_viewport_size();
		auto g_buffer_fbo = render_view.get_g_buffer_fbo(viewport_size);

		RenderPass pass("g_buffer_fill");
		pass.bind(g_buffer_fbo.get());
		pass.clear();
		pass.set_view_proj(view, proj);

		for(auto& element : visibility_set)
		{
			auto& e = std::get<0>(element);
			auto& transform_comp_handle = std::get<1>(element);
			auto& model_comp_handle = std::get<2>(element);
			auto transform_comp_ptr = transform_comp_handle.lock();
			auto model_comp_ptr = model_comp_handle.lock();
			if(!transform_comp_ptr || !model_comp_ptr)
				continue;

			auto& transform_comp_ref = *transform_comp_ptr.get();
			auto& model_comp_ref = *model_comp_ptr.get();

			const auto& model = model_comp_ref.get_model();
			if (!model.is_valid())
				continue;

			const auto& world_transform = transform_comp_ref.get_transform();
			const auto clip_planes = math::vec2(camera.get_near_clip(), camera.get_far_clip());

			auto& lod_data = camera_lods[e];
			const auto transition_time = model.get_lod_transition_time();
			const auto min_distance = model.get_lod_min_distance();
			const auto max_distance = model.get_lod_max_distance();
			const auto lod_count = model.get_lods().size();
			const auto current_time = lod_data.current_time;
			const auto current_lod_index = lod_data.current_lod_index;
			const auto target_lod_index = lod_data.target_lod_index;

			const auto current_mesh = model.get_lod(current_lod_index);
			if (!current_mesh)
				continue;;

			if (model.get_lods().size() > 1)
			{
				const auto& bounds = current_mesh->get_bounds();

				float t = 0.0f;
				const auto ray_origin = camera.get_position();
				const auto inv_world = math::inverse(world_transform);
				const auto object_ray_origin = inv_world.transform_coord(ray_origin);
				const auto object_ray_direction = math::normalize(bounds.get_center() - object_ray_origin);
				bounds.intersect(object_ray_origin, object_ray_direction, t);

				// Compute final object space intersection point.
				auto intersection_point = object_ray_origin + (object_ray_direction * t);

				// transform intersection point back into world space to compute
				// the final intersection distance.
				intersection_point = world_transform.transform_coord(intersection_point);
				const float distance = math::length(intersection_point - ray_origin);

				//Compute Lods
				update_lod_data(
					lod_data,
					lod_count,
					min_distance,
					max_distance,
					transition_time,
					distance,
					dt.count());
			}
			
			const auto params = math::vec3{
				0.0f,
				-1.0f,
				(transition_time - current_time) / transition_time
			};

			const auto params_inv = math::vec3{
				1.0f,
				1.0f,
				current_time / transition_time
			};

			model.render(
				pass.id,
				world_transform,
				true,
				true,
				true,
				0,
				current_lod_index,
				nullptr,
				[&camera, &clip_planes, &params](Program& program)
			{
				program.set_uniform("u_camera_wpos", &camera.get_position());
				program.set_uniform("u_camera_clip_planes", &clip_planes);
				program.set_uniform("u_lod_params", &params);
			});

			if (current_time != 0.0f)
			{
				model.render(
					pass.id,
					world_transform,
					true,
					true,
					true,
					0,
					target_lod_index,
					nullptr,
					[&camera, &clip_planes, &params_inv](Program& program)
				{
					program.set_uniform("u_lod_params", &params_inv);
				});
			}

		}

		return g_buffer_fbo;
	}

	std::shared_ptr<FrameBuffer> DeferredRendering::lighting_pass(
		std::shared_ptr<FrameBuffer> input, 
		Camera& camera, 
		RenderView& render_view,
		EntityComponentSystem& ecs,
		std::chrono::duration<float> dt,
		bool bind_indirect_specular)
	{
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		const auto view_proj = proj * view;
		const auto inv_view_proj = math::inverse(view_proj);

		const auto& viewport_size = camera.get_viewport_size();
		auto g_buffer_fbo = render_view.get_g_buffer_fbo(viewport_size).get();

		static auto light_buffer_format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha |
			gfx::FormatSearchFlags::HalfPrecisionFloat);

		auto light_buffer = render_view.get_texture("LBUFFER", viewport_size.width, viewport_size.height, false, 1, light_buffer_format);
		auto l_buffer_fbo = render_view.get_fbo("LBUFFER", { light_buffer });
		const auto buffer_size = l_buffer_fbo->get_size();

		RenderPass pass("light_buffer_fill");
		pass.bind(l_buffer_fbo.get());
		pass.clear(BGFX_CLEAR_COLOR, 0, 0.0f, 0);
		pass.set_view_proj(view, proj);

		auto refl_buffer = render_view.get_texture("RBUFFER", viewport_size.width, viewport_size.height, false, 1, light_buffer_format).get();

		ecs.each<TransformComponent, LightComponent>([this, bind_indirect_specular, &camera, &pass, &buffer_size, &view, &proj, g_buffer_fbo, refl_buffer](
			Entity e,
			TransformComponent& transform_comp_ref,
			LightComponent& light_comp_ref
			)
		{
			const auto& light = light_comp_ref.get_light();
			const auto& world_transform = transform_comp_ref.get_transform();
			const auto& light_position = world_transform.get_position();
			const auto& light_direction = world_transform.z_unit_axis();

			iRect rect(0, 0, buffer_size.width, buffer_size.height);
			if (light_comp_ref.compute_projected_sphere_rect(rect, light_position, light_direction, view, proj) == 0)
				return;


			Program* program = nullptr;
			if (light.light_type == LightType::Directional && _directional_light_program)
			{
				// Draw light.
				program = _directional_light_program.get();
				program->begin_pass();
				program->set_uniform("u_light_direction", &light_direction);
			}
			if (light.light_type == LightType::Point && _point_light_program)
			{
				float light_data[4] =
				{
					light.point_data.range,
					light.point_data.exponent_falloff,
					0.0f,
					0.0f
				};

				// Draw light.
				program = _point_light_program.get();
				program->begin_pass();
				program->set_uniform("u_light_position", &light_position);
				program->set_uniform("u_light_data", light_data);
			}

			if (light.light_type == LightType::Spot && _spot_light_program)
			{
				float light_data[4] =
				{
					light.spot_data.get_range(),
					math::cos(math::radians(light.spot_data.get_inner_angle() * 0.5f)),
					math::cos(math::radians(light.spot_data.get_outer_angle() * 0.5f)),
					0.0f
				};

				// Draw light.
				program = _spot_light_program.get();
				program->begin_pass();
				program->set_uniform("u_light_position", &light_position);
				program->set_uniform("u_light_direction", &light_direction);
				program->set_uniform("u_light_data", light_data);		
			}

			if (program)
			{
				float light_color_intensity[4] =
				{
					light.color.value.r,
					light.color.value.g,
					light.color.value.b,
					light.intensity
				};
				program->set_uniform("u_light_color_intensity", light_color_intensity);
				program->set_uniform("u_camera_position", &camera.get_position());
				program->set_texture(0, "s_tex0", gfx::getTexture(g_buffer_fbo->handle, 0));
				program->set_texture(1, "s_tex1", gfx::getTexture(g_buffer_fbo->handle, 1));
				program->set_texture(2, "s_tex2", gfx::getTexture(g_buffer_fbo->handle, 2));
				program->set_texture(3, "s_tex3", gfx::getTexture(g_buffer_fbo->handle, 3));
				program->set_texture(4, "s_tex4", gfx::getTexture(g_buffer_fbo->handle, 4));
				program->set_texture(5, "s_tex5", refl_buffer->handle);

				gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
				auto topology = gfx::clip_quad(1.0f);
				gfx::setState(topology
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
					| BGFX_STATE_BLEND_ADD
				);
				gfx::submit(pass.id, program->handle);
				gfx::setState(BGFX_STATE_DEFAULT);
			}
		});

		return l_buffer_fbo;
	}

	std::shared_ptr<FrameBuffer> DeferredRendering::reflection_probe_pass(
		std::shared_ptr<FrameBuffer> input,
		Camera& camera,
		RenderView& render_view,
		EntityComponentSystem& ecs,
		std::chrono::duration<float> dt)
	{
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();

		const auto& viewport_size = camera.get_viewport_size();
		auto g_buffer_fbo = render_view.get_g_buffer_fbo(viewport_size).get();


		static auto refl_buffer_format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha |
			gfx::FormatSearchFlags::HalfPrecisionFloat);

		auto refl_buffer = render_view.get_texture("RBUFFER", viewport_size.width, viewport_size.height, false, 1, refl_buffer_format);
		auto r_buffer_fbo = render_view.get_fbo("RBUFFER", { refl_buffer });
		const auto buffer_size = refl_buffer->get_size();

		RenderPass pass("refl_buffer_fill");
		pass.bind(r_buffer_fbo.get());
		pass.clear(BGFX_CLEAR_COLOR, 0, 0.0f, 0);
		pass.set_view_proj(view, proj);


		ecs.each<TransformComponent, ReflectionProbeComponent>([this, &camera, &pass, &buffer_size, &view, &proj, g_buffer_fbo](
			Entity e,
			TransformComponent& transform_comp_ref,
			ReflectionProbeComponent& probe_comp_ref
			)
		{
			const auto& probe = probe_comp_ref.get_probe();
			const auto& world_transform = transform_comp_ref.get_transform();
			const auto& probe_position = world_transform.get_position();

			iRect rect(0, 0, buffer_size.width, buffer_size.height);
			if (probe_comp_ref.compute_projected_sphere_rect(rect, probe_position, view, proj) == 0)
				return;

			const auto cubemap = probe_comp_ref.get_cubemap();

			Program* program = nullptr;
			float influence_radius = 0.0f;
			if (probe.probe_type == ProbeType::Sphere && _sphere_ref_probe_program)
			{
				program = _sphere_ref_probe_program.get();
				program->begin_pass();
				influence_radius = probe.sphere_data.range;
			}

			if (probe.probe_type == ProbeType::Box && _box_ref_probe_program)
			{
				math::transform_t t;
				t.set_scale(probe.box_data.extents);
				t = world_transform * t;
				auto u_inv_world = math::inverse(t);
				float data2[4] =
				{
					probe.box_data.extents.x,
					probe.box_data.extents.y,
					probe.box_data.extents.z,
					probe.box_data.transition_distance
				};

				program = _box_ref_probe_program.get();
				program->begin_pass();
				program->set_uniform("u_inv_world", &u_inv_world);
				program->set_uniform("u_data2", data2);
				
				influence_radius = math::length(t.get_scale() + probe.box_data.transition_distance);
			}

			if (program)
			{
				float mips = cubemap ? float(cubemap->info.numMips) : 1.0f;
				float data0[4] =
				{
					probe_position.x,
					probe_position.y,
					probe_position.z,
					influence_radius,
				};

				float data1[4] =
				{
					mips, 0.0f, 0.0f, 0.0f
				};

				program->set_uniform("u_data0", data0);
				program->set_uniform("u_data1", data1);
				
				program->set_texture(0, "s_tex0", gfx::getTexture(g_buffer_fbo->handle, 0));
				program->set_texture(1, "s_tex1", gfx::getTexture(g_buffer_fbo->handle, 1));
				program->set_texture(2, "s_tex2", gfx::getTexture(g_buffer_fbo->handle, 2));
				program->set_texture(3, "s_tex3", gfx::getTexture(g_buffer_fbo->handle, 3));
				program->set_texture(4, "s_tex4", gfx::getTexture(g_buffer_fbo->handle, 4));
				program->set_texture(5, "s_tex_cube", cubemap->handle);
				gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
				auto topology = gfx::clip_quad(1.0f);
				gfx::setState(topology
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
					| BGFX_STATE_BLEND_ALPHA
				);
				gfx::submit(pass.id, program->handle);
				gfx::setState(BGFX_STATE_DEFAULT);
			}
		});

		return r_buffer_fbo;
	}

	std::shared_ptr<FrameBuffer> DeferredRendering::atmospherics_pass(
		std::shared_ptr<FrameBuffer> input,
		Camera& camera,
		RenderView& render_view,
		EntityComponentSystem& ecs,
		std::chrono::duration<float> dt)
	{
		auto far_clip_cache = camera.get_far_clip();
		camera.set_far_clip(1000.0f);
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		camera.set_far_clip(far_clip_cache);
		const auto& viewport_size = camera.get_viewport_size();
	
		static auto light_buffer_format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha |
			gfx::FormatSearchFlags::HalfPrecisionFloat);

		auto light_buffer = render_view.get_texture("LBUFFER", viewport_size.width, viewport_size.height, false, 1, light_buffer_format, gfx::get_default_rt_sampler_flags());
		input = render_view.get_fbo("LBUFFER", { light_buffer, render_view.get_depth_stencil_buffer(viewport_size) });
	
		const auto surface = input.get();
		const auto output_size = surface->get_size();
		RenderPass pass("atmospherics_fill");
		pass.bind(surface);
		pass.set_view_proj(view, proj);

		if (surface && _atmospherics_program)
		{
			ecs.each<TransformComponent, LightComponent>([this, &output_size, &pass](
				Entity e,
				TransformComponent& transform_comp_ref,
				LightComponent& light_comp_ref
				)
			{
				const auto& light = light_comp_ref.get_light();
				const auto& world_transform = transform_comp_ref.get_transform();
				const auto& light_direction = world_transform.z_unit_axis();

				if (light.light_type == LightType::Directional)
				{
					_atmospherics_program->begin_pass();
					_atmospherics_program->set_uniform("u_light_direction", &light_direction);

					iRect rect(0, 0, output_size.width, output_size.height);
					gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
					auto topology = gfx::clip_quad(1.0f);
					gfx::setState(topology
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_DEPTH_TEST_LEQUAL
						| BGFX_STATE_BLEND_ADD
					);
					gfx::submit(pass.id, _atmospherics_program->handle);
					gfx::setState(BGFX_STATE_DEFAULT);
				}
			});
		}

		return input;
	}

	std::shared_ptr<FrameBuffer> DeferredRendering::tonemapping_pass(
		std::shared_ptr<FrameBuffer> input,
		Camera& camera,
		RenderView& render_view)
	{
		if (!input)
			return nullptr;

		const auto& viewport_size = camera.get_viewport_size();
		const auto surface = render_view.get_output_fbo(viewport_size);
		const auto output_size = surface->get_size();
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		RenderPass pass("output_buffer_fill");
		pass.bind(surface.get());
		pass.set_view_proj(view, proj);

		if (surface && _gamma_correction_program)
		{
			_gamma_correction_program->begin_pass();
			_gamma_correction_program->set_texture(0, "s_input", gfx::getTexture(input->handle));
			iRect rect(0, 0, output_size.width, output_size.height);
			gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
			auto topology = gfx::clip_quad(1.0f);
			gfx::setState(topology
				| BGFX_STATE_RGB_WRITE
				| BGFX_STATE_ALPHA_WRITE
			);
			gfx::submit(pass.id, _gamma_correction_program->handle);
			gfx::setState(BGFX_STATE_DEFAULT);
		}

		return surface;
	}

	void DeferredRendering::receive(Entity e)
	{
		_lod_data.erase(e);
		for (auto& pair : _lod_data)
		{
			pair.second.erase(e);
		}
	}
	bool DeferredRendering::initialize()
	{
		on_entity_destroyed.connect(this, &DeferredRendering::receive);
		on_frame_render.connect(this, &DeferredRendering::frame_render);

		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<Shader>("engine_data:/shaders/vs_clip_quad", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_deferred_point_light", false)
				.then([this, vs](auto fs)
			{
				_point_light_program = std::make_unique<Program>(vs, fs);
			});

			am->load<Shader>("engine_data:/shaders/fs_deferred_spot_light", false)
				.then([this, vs](auto fs)
			{
				_spot_light_program = std::make_unique<Program>(vs, fs);
			});

			am->load<Shader>("engine_data:/shaders/fs_deferred_directional_light", false)
				.then([this, vs](auto fs)
			{
				_directional_light_program = std::make_unique<Program>(vs, fs);
			});

			am->load<Shader>("engine_data:/shaders/fs_gamma_correction", false)
				.then([this, vs](auto fs)
			{
				_gamma_correction_program = std::make_unique<Program>(vs, fs);
			});
		});

		am->load<Shader>("engine_data:/shaders/vs_clip_quad_ex", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_sphere_reflection_probe", false)
				.then([this, vs](auto fs)
			{
				_sphere_ref_probe_program = std::make_unique<Program>(vs, fs);
			});

			am->load<Shader>("engine_data:/shaders/fs_box_reflection_probe", false)
				.then([this, vs](auto fs)
			{
				_box_ref_probe_program = std::make_unique<Program>(vs, fs);
			});
			am->load<Shader>("engine_data:/shaders/fs_atmospherics", false)
				.then([this, vs](auto fs)
			{
				_atmospherics_program = std::make_unique<Program>(vs, fs);
			});
		});

		return true;
	}

	void DeferredRendering::dispose()
	{
		on_entity_destroyed.disconnect(this, &DeferredRendering::receive);
		on_frame_render.disconnect(this, &DeferredRendering::frame_render);
	}


}