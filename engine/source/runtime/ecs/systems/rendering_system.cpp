#include "rendering_system.h"
#include "../components/transform_component.h"
#include "../components/camera_component.h"
#include "../components/model_component.h"
#include "../components/light_component.h"
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

	void screen_space_quad(float _textureWidth, float _textureHeight, float _texelHalf, bool _originBottomLeft, float _width = 1.0f, float _height = 1.0f)
	{
		if (3 == gfx::getAvailTransientVertexBuffer(3, gfx::PosTexCoord0Vertex::decl))
		{
			gfx::TransientVertexBuffer vb;
			gfx::allocTransientVertexBuffer(&vb, 3, gfx::PosTexCoord0Vertex::decl);
			gfx::PosTexCoord0Vertex* vertex = (gfx::PosTexCoord0Vertex*)vb.data;

			const float minx = -_width;
			const float maxx = _width;
			const float miny = 0.0f;
			const float maxy = _height*2.0f;

			const float texelHalfW = _texelHalf / _textureWidth;
			const float texelHalfH = _texelHalf / _textureHeight;
			const float minu = -1.0f + texelHalfW;
			const float maxu = 1.0f + texelHalfH;

			const float zz = 0.0f;

			float minv = texelHalfH;
			float maxv = 2.0f + texelHalfH;

			if (_originBottomLeft)
			{
				float temp = minv;
				minv = maxv;
				maxv = temp;

				minv -= 1.0f;
				maxv -= 1.0f;
			}

			vertex[0].x = minx;
			vertex[0].y = miny;
			vertex[0].z = zz;
			vertex[0].u = minu;
			vertex[0].v = minv;

			vertex[1].x = maxx;
			vertex[1].y = miny;
			vertex[1].z = zz;
			vertex[1].u = maxu;
			vertex[1].v = minv;

			vertex[2].x = maxx;
			vertex[2].y = maxy;
			vertex[2].z = zz;
			vertex[2].u = maxu;
			vertex[2].v = maxv;

			gfx::setVertexBuffer(&vb);
		}
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

	void RenderingSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto ecs = core::get_subsystem<EntityComponentSystem>();
		ecs->each<CameraComponent>([this, ecs, dt](
			Entity ce,
			CameraComponent& camera_comp
			)
		{
			const auto g_buffer = camera_comp.get_g_buffer().get();
			auto& camera = camera_comp.get_camera();
			const auto& view = camera.get_view();
			const auto& proj = camera.get_projection();
			const auto view_proj = proj * view;
			const auto inv_view_proj = math::inverse(view_proj);

			FrameBuffer* fbo = g_buffer;
			RenderPass geometry_pass("g_buffer_fill");
			geometry_pass.bind(g_buffer);
			geometry_pass.clear();
			auto& camera_lods = _lod_data[ce];

			gfx::setViewTransform(geometry_pass.id, &view, &proj);

			ecs->each<TransformComponent, ModelComponent>([this, &camera_lods, &camera, dt, &geometry_pass](
				Entity e,
				TransformComponent& transform_comp_ref,
				ModelComponent& model_comp_ref
				)
			{
				const auto& model = model_comp_ref.get_model();
				if (!model.is_valid())
					return;

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
					return;

				const auto& frustum = camera.get_frustum();
				const auto& bounds = current_mesh->aabb;

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

				// Test the bounding box of the mesh
				if (!math::frustum::test_obb(frustum, bounds, world_transform))
					return;

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
					geometry_pass.id,
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
						geometry_pass.id,
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

			});

			const auto light_buffer = camera_comp.get_light_buffer().get();
			const auto light_buffer_size = light_buffer->get_size();
			fbo = light_buffer;
			RenderPass light_pass("light_buffer_fill");
			light_pass.bind(light_buffer);
			light_pass.clear();

			const math::transform_t ortho_proj = math::ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, camera.get_far_clip(), gfx::getCaps()->homogeneousDepth);
			gfx::setViewTransform(light_pass.id, nullptr, &ortho_proj);

			ecs->each<TransformComponent, LightComponent>([this, &camera, dt, &light_pass, &light_buffer_size, &view, &proj, &view_proj, &inv_view_proj, g_buffer](
				Entity e,
				TransformComponent& transform_comp_ref,
				LightComponent& light_comp_ref
				)
			{
				auto& light = light_comp_ref.get_light();
				const auto& world_transform = transform_comp_ref.get_transform();
				const auto& light_position = world_transform.get_position();
				const auto& light_direction = world_transform.z_unit_axis();

				iRect rect(0, 0, light_buffer_size.width, light_buffer_size.height);
				if (light_comp_ref.compute_projected_sphere_rect(rect, light_position, light_direction, view, proj) == 0)
					return;

				if (light.light_type == LightType::Directional && _directional_light_program)
 				{
					float light_color_intensity[4] =
					{
						light.color.value.r,
						light.color.value.g,
						light.color.value.b,
						light.intensity
					};

					// Draw light.
					_directional_light_program->begin_pass();
					_directional_light_program->set_uniform("u_light_direction", &light_direction);
					_directional_light_program->set_uniform("u_light_color_intensity", light_color_intensity);
					_directional_light_program->set_uniform("u_camera_position", &camera.get_position());
					_directional_light_program->set_uniform("u_mtx", &inv_view_proj);
					_directional_light_program->set_texture(0, "s_albedo", gfx::getTexture(g_buffer->handle, 0));
					_directional_light_program->set_texture(1, "s_normal", gfx::getTexture(g_buffer->handle, 1));
					_directional_light_program->set_texture(2, "s_emissive", gfx::getTexture(g_buffer->handle, 2));
					_directional_light_program->set_texture(3, "s_depth", gfx::getTexture(g_buffer->handle, 3));

					gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
					gfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_BLEND_ADD
					);
					const gfx::RendererType::Enum renderer = gfx::getRendererType();
					float half_texel = gfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
					screen_space_quad((float)light_buffer_size.width, (float)light_buffer_size.height, half_texel, bgfx::getCaps()->originBottomLeft);
					gfx::submit(light_pass.id, _directional_light_program->handle);
				}
				if (light.light_type == LightType::Point && _point_light_program)
				{

					float light_color_intensity[4] =
					{
						light.color.value.r,
						light.color.value.g,
						light.color.value.b,
						light.intensity
					};

					float light_data[4] =
					{
						light.point_data.range,
						light.point_data.exponent_falloff,
						0.0f,
						0.0f
					};

					// Draw light.
					_point_light_program->begin_pass();
					_point_light_program->set_uniform("u_light_position", &light_position);
					_point_light_program->set_uniform("u_light_color_intensity", light_color_intensity);
					_point_light_program->set_uniform("u_light_data", light_data);
					_point_light_program->set_uniform("u_camera_position", &camera.get_position());
					_point_light_program->set_uniform("u_mtx", &inv_view_proj);
					_point_light_program->set_texture(0, "s_albedo", gfx::getTexture(g_buffer->handle, 0));
					_point_light_program->set_texture(1, "s_normal", gfx::getTexture(g_buffer->handle, 1));
					_point_light_program->set_texture(2, "s_emissive", gfx::getTexture(g_buffer->handle, 2));
					_point_light_program->set_texture(3, "s_depth", gfx::getTexture(g_buffer->handle, 3));

					gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
					gfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_BLEND_ADD
					);
					const gfx::RendererType::Enum renderer = gfx::getRendererType();
					float half_texel = gfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
					screen_space_quad((float)light_buffer_size.width, (float)light_buffer_size.height, half_texel, bgfx::getCaps()->originBottomLeft);
					gfx::submit(light_pass.id, _point_light_program->handle);
					
				}

				if (light.light_type == LightType::Spot && _spot_light_program)
				{

					float light_color_intensity[4] =
					{
						light.color.value.r,
						light.color.value.g,
						light.color.value.b,
						light.intensity
					};

					float light_data[4] =
					{
						light.spot_data.get_range(),
						math::cos(math::radians(light.spot_data.get_inner_angle() * 0.5f)),
						math::cos(math::radians(light.spot_data.get_outer_angle() * 0.5f)),
						0.0f
					};

					// Draw light.
					_spot_light_program->begin_pass();
					_spot_light_program->set_uniform("u_light_position", &light_position);
					_spot_light_program->set_uniform("u_light_direction", &light_direction);
					_spot_light_program->set_uniform("u_light_color_intensity", light_color_intensity);
					_spot_light_program->set_uniform("u_light_data", light_data);
					_spot_light_program->set_uniform("u_camera_position", &camera.get_position());
					_spot_light_program->set_uniform("u_mtx", &inv_view_proj);
					_spot_light_program->set_texture(0, "s_albedo", gfx::getTexture(g_buffer->handle, 0));
					_spot_light_program->set_texture(1, "s_normal", gfx::getTexture(g_buffer->handle, 1));
					_spot_light_program->set_texture(2, "s_emissive", gfx::getTexture(g_buffer->handle, 2));
					_spot_light_program->set_texture(3, "s_depth", gfx::getTexture(g_buffer->handle, 3));

					gfx::setScissor(rect.left, rect.top, rect.width(), rect.height());
					gfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_BLEND_ADD
					);
					const gfx::RendererType::Enum renderer = gfx::getRendererType();
					float half_texel = gfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
					screen_space_quad((float)light_buffer_size.width, (float)light_buffer_size.height, half_texel, bgfx::getCaps()->originBottomLeft);
					gfx::submit(light_pass.id, _spot_light_program->handle);
				}
			});


			const auto surface = camera_comp.get_output_buffer().get();
			const auto output_size = surface->get_size();
			RenderPass pass_blit("output_buffer_fill");
			pass_blit.bind(surface);
			gfx::setScissor(0, 0, output_size.width, output_size.height);
			gfx::setViewTransform(pass_blit.id, nullptr, &ortho_proj);
			if (fbo && surface && _gamma_correction_program)
			{
				_gamma_correction_program->begin_pass();
				_gamma_correction_program->set_texture(0, "s_input", gfx::getTexture(fbo->handle));
				
				gfx::setState(0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
				);
				const gfx::RendererType::Enum renderer = gfx::getRendererType();
				float half_texel = gfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
				screen_space_quad((float)output_size.width, (float)output_size.height, half_texel, bgfx::getCaps()->originBottomLeft);
				gfx::submit(pass_blit.id, _gamma_correction_program->handle);

				//gfx::blit(pass_blit.id, gfx::getTexture(surface->handle), 0, 0, gfx::getTexture(fbo->handle));
			}
		});
	}

	void RenderingSystem::receive(Entity e)
	{
		_lod_data.erase(e);
		for (auto& pair : _lod_data)
		{
			pair.second.erase(e);
		}
	}



	bool RenderingSystem::initialize()
	{
		onEntityDestroyed.connect(this, &RenderingSystem::receive);
		on_frame_render.connect(this, &RenderingSystem::frame_render);

		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<Shader>("engine_data:/shaders/vs_deferred_point_light", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_deferred_point_light", false)
				.then([this, vs](auto fs)
			{
				_point_light_program = std::make_unique<Program>(vs, fs);
			});
		});

		am->load<Shader>("engine_data:/shaders/vs_deferred_spot_light", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_deferred_spot_light", false)
				.then([this, vs](auto fs)
			{
				_spot_light_program = std::make_unique<Program>(vs, fs);
			});
		});

		am->load<Shader>("engine_data:/shaders/vs_deferred_directional_light", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_deferred_directional_light", false)
				.then([this, vs](auto fs)
			{
				_directional_light_program = std::make_unique<Program>(vs, fs);
			});
		});

		am->load<Shader>("engine_data:/shaders/vs_gamma_correction", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("engine_data:/shaders/fs_gamma_correction", false)
				.then([this, vs](auto fs)
			{
				_gamma_correction_program = std::make_unique<Program>(vs, fs);
			});
		});

		return true;
	}

	void RenderingSystem::dispose()
	{
		onEntityDestroyed.disconnect(this, &RenderingSystem::receive);
		on_frame_render.disconnect(this, &RenderingSystem::frame_render);
	}
}