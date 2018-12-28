#include "picking_system.h"
#include "editing_system.h"

#include <core/graphics/render_pass.h>
#include <core/graphics/texture.h>
#include <core/logging/logging.h>
#include <core/system/subsystem.h>

#include <runtime/assets/asset_manager.h>
#include <runtime/ecs/components/camera_component.h>
#include <runtime/ecs/components/model_component.h>
#include <runtime/ecs/components/transform_component.h>
#include <runtime/input/input.h>
#include <runtime/rendering/camera.h>
#include <runtime/rendering/material.h>
#include <runtime/rendering/mesh.h>
#include <runtime/rendering/model.h>
#include <runtime/rendering/render_window.h>
#include <runtime/rendering/renderer.h>
#include <runtime/system/events.h>

namespace editor
{
constexpr int picking_system::tex_id_dim;

void picking_system::frame_render(delta_t dt)
{
	auto& es = core::get_subsystem<editing_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& renderer = core::get_subsystem<runtime::renderer>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();

	const auto render_frame = renderer.get_render_frame();

	if(input.is_mouse_button_pressed(mml::mouse::left))
	{
		auto& editor_camera = es.camera;
		if(imguizmo::is_over() && es.selection_data.object)
			return;

		if(!editor_camera || !editor_camera.has_component<camera_component>())
			return;

		auto camera_comp = editor_camera.get_component<camera_component>();
		auto camera_comp_ptr = camera_comp.lock().get();
		const auto& current_camera = camera_comp_ptr->get_camera();
		const auto near_clip = current_camera.get_near_clip();
		const auto far_clip = current_camera.get_far_clip();
		const auto& mouse_pos = input.get_current_cursor_position();
		const auto& frustum = current_camera.get_frustum();
		math::vec2 cursor_pos = math::vec2{mouse_pos.x, mouse_pos.y};
		math::vec3 pick_eye;
		math::vec3 pick_at;
		math::vec3 pick_up = {0.0f, 1.0f, 0.0f};

		if(!current_camera.viewport_to_world(cursor_pos, frustum.planes[math::volume_plane::near_plane],
											 pick_eye, true))
			return;

		if(!current_camera.viewport_to_world(cursor_pos, frustum.planes[math::volume_plane::far_plane],
											 pick_at, true))
			return;

		reading_ = 0;
		start_readback_ = true;

		camera pick_camera;
		pick_camera.set_aspect_ratio(1.0f);
		pick_camera.set_fov(1.0f);
		pick_camera.set_near_clip(near_clip);
		pick_camera.set_far_clip(far_clip);
		pick_camera.look_at(pick_eye, pick_at, pick_up);

		const auto& pick_view = pick_camera.get_view();
		const auto& pick_proj = pick_camera.get_projection();
		const auto& pick_frustum = pick_camera.get_frustum();

		gfx::render_pass pass("picking_buffer_fill");
		// ID buffer clears to black, which represents clicking on nothing (background)
		pass.clear(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
		pass.set_view_proj(pick_view, pick_proj);
		pass.bind(surface_.get());

		ecs.for_each<transform_component, model_component>(
			[this, &pass, &pick_frustum](runtime::entity e, transform_component& transform_comp_ref,
										 model_component& model_comp_ref) {
				auto& model = model_comp_ref.get_model();
				if(!model.is_valid())
					return;

				const auto& world_transform = transform_comp_ref.get_transform();

				auto mesh = model.get_lod(0);
				if(!mesh)
					return;

				const auto& bounds = mesh->get_bounds();

				// Test the bounding box of the mesh
				if(!math::frustum::test_obb(pick_frustum, bounds, world_transform))
					return;

				auto entity_index = e.id().index();
				std::uint32_t rr = (entity_index)&0xff;
				std::uint32_t gg = (entity_index >> 8) & 0xff;
				std::uint32_t bb = (entity_index >> 16) & 0xff;
				math::vec4 color_id = {rr / 255.0f, gg / 255.0f, bb / 255.0f, 1.0f};

				const auto& bone_transforms = model_comp_ref.get_bone_transforms();
				model.render(pass.id, world_transform, bone_transforms, true, true, true, 0, 0,
							 program_.get(), [&color_id](auto& p) { p.set_uniform("u_id", &color_id); });
			});
	}

	// If the user previously clicked, and we're done reading data from GPU, look at ID buffer on CPU
	// Whatever mesh has the most pixels in the ID buffer is the one the user clicked on.
	if((reading_ == 0u) && start_readback_)
	{
		bool blit_support = gfx::is_supported(BGFX_CAPS_TEXTURE_BLIT);

		if(blit_support == false)
		{
			APPLOG_WARNING("Texture blitting is not supported. Picking will not work");
			start_readback_ = false;
			return;
		}

		gfx::render_pass pass("picking_buffer_blit");
		pass.touch();
		// Blit and read
		gfx::blit(pass.id, blit_tex_->native_handle(), 0, 0, surface_->get_texture()->native_handle());
		reading_ = gfx::read_texture(blit_tex_->native_handle(), blit_data_);
		start_readback_ = false;
	}

	if(reading_ && reading_ <= render_frame)
	{
		reading_ = 0;
		std::map<std::uint32_t, std::uint32_t> ids; // This contains all the IDs found in the buffer
		std::uint32_t max_amount = 0;
		for(std::uint8_t* x = blit_data_; x < blit_data_ + tex_id_dim * tex_id_dim * 4;)
		{
			std::uint8_t rr = *x++;
			std::uint8_t gg = *x++;
			std::uint8_t bb = *x++;
			std::uint8_t aa = *x++;
			(void)aa;
			if(gfx::renderer_type::Direct3D9 == gfx::get_renderer_type())
			{
				// Comes back as BGRA
				std::swap(rr, bb);
			}

			// Skip background
			if(0 == (rr | gg | bb))
			{
				continue;
			}

			auto hash_key = static_cast<std::uint32_t>(rr + (gg << 8) + (bb << 16));
			std::uint32_t amount = 1;
			auto mapIter = ids.find(hash_key);
			if(mapIter != ids.end())
			{
				amount = mapIter->second + 1;
			}

			// Amount of times this ID (color) has been clicked on in buffer
			ids[hash_key] = amount;
			max_amount = max_amount > amount ? max_amount : amount;
		}

		std::uint32_t id_key = 0;
		if(max_amount != 0u)
		{
			for(auto& pair : ids)
			{
				if(pair.second == max_amount)
				{
					id_key = pair.first;
					if(ecs.valid_index(id_key))
					{
						auto eid = ecs.create_id(id_key);
						auto picked_entity = ecs.get(eid);
						if(picked_entity)
							es.select(picked_entity);
					}
					break;
				}
			}
		}
		else
		{
			es.unselect();
		}
	}
}

picking_system::picking_system()
{
	runtime::on_frame_render.connect(this, &picking_system::frame_render);
	// Set up ID buffer, which has a color target and depth buffer
	auto picking_rt = std::make_shared<gfx::texture>(
		tex_id_dim, tex_id_dim, false, 1, gfx::texture_format::RGBA8,
		0
		| BGFX_TEXTURE_RT
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP);

	auto picking_rt_depth = std::make_shared<gfx::texture>(
		tex_id_dim, tex_id_dim, false, 1, gfx::texture_format::D24S8,
		0
		| BGFX_TEXTURE_RT
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP);

    std::vector<std::shared_ptr<gfx::texture>> textures{picking_rt, picking_rt_depth};
	surface_ = std::make_shared<gfx::frame_buffer>(textures);

	// CPU texture for blitting to and reading ID buffer so we can see what was clicked on.
	// Impossible to read directly from a render target, you *must* blit to a CPU texture
	// first. Algorithm Overview: Render on GPU -> Blit to CPU texture -> Read from CPU
	// texture.
	blit_tex_ = std::make_shared<gfx::texture>(
		tex_id_dim, tex_id_dim, false, 1, gfx::texture_format::RGBA8,
		0
		| BGFX_TEXTURE_BLIT_DST
		| BGFX_TEXTURE_READ_BACK
		| BGFX_SAMPLER_MIN_POINT
		| BGFX_SAMPLER_MAG_POINT
		| BGFX_SAMPLER_MIP_POINT
		| BGFX_SAMPLER_U_CLAMP
		| BGFX_SAMPLER_V_CLAMP);

	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();

	auto vs_picking_id = am.load<gfx::shader>("editor:/data/shaders/vs_picking_id.sc");
	vs_picking_id.wait();
	auto fs_picking_id = am.load<gfx::shader>("editor:/data/shaders/fs_picking_id.sc");
	fs_picking_id.wait();
	ts.push_or_execute_on_owner_thread(
		[this](asset_handle<gfx::shader> vs, asset_handle<gfx::shader> fs) {
			program_ = std::make_unique<gpu_program>(vs, fs);

		},
		vs_picking_id, fs_picking_id);
}

picking_system::~picking_system()
{
	runtime::on_frame_render.disconnect(this, &picking_system::frame_render);
}
}
