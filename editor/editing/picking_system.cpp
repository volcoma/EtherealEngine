#include "picking_system.h"
#include "editing_system.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/input/input.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/model.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/render_window.h"
#include "runtime/rendering/renderer.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/system/engine.h"

namespace editor
{
constexpr int picking_system::tex_id_dim;

void picking_system::frame_render(std::chrono::duration<float>)
{
	auto& es = core::get_subsystem<editing_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& renderer = core::get_subsystem<runtime::renderer>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();

	const auto render_frame = renderer.get_render_frame();

	auto& editor_camera = es.camera;
	if(imguizmo::is_over() && es.selection_data.object)
		return;

	if(!editor_camera || !editor_camera.has_component<camera_component>())
		return;

	auto camera_comp = editor_camera.get_component<camera_component>();
	auto camera_comp_ptr = camera_comp.lock().get();
	auto& camera = camera_comp_ptr->get_camera();
	auto near_clip = camera.get_near_clip();
	auto far_clip = camera.get_far_clip();
	const auto& mouse_pos = input.get_current_cursor_position();
	const auto& frustum = camera.get_frustum();
	math::vec2 cursor_pos = math::vec2{mouse_pos.x, mouse_pos.y};
	math::vec3 pick_eye;
	math::vec3 pick_at;
	math::vec3 pick_up = {0.0f, 1.0f, 0.0f};

	if(!camera.viewport_to_world(cursor_pos, frustum.planes[math::volume_plane::near_plane], pick_eye, true))
		return;

	if(!camera.viewport_to_world(cursor_pos, frustum.planes[math::volume_plane::far_plane], pick_at, true))
		return;

	if(input.is_mouse_button_pressed(mml::mouse::left))
	{
		_reading = 0;
		_start_readback = true;
		auto pick_view = math::lookAt(pick_eye, pick_at, pick_up);
		auto pick_proj =
			math::perspective(math::radians(1.0f), 1.0f, near_clip, far_clip, gfx::is_homogeneous_depth());

		render_pass pass("picking_buffer_fill");
		pass.bind(_surface.get());
		// ID buffer clears to black, which represents clicking on nothing (background)
		pass.clear(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

		pass.set_view_proj(pick_view, pick_proj);

		ecs.each<transform_component, model_component>([this, &pass, &camera](
			runtime::entity e, transform_component& transform_comp_ref, model_component& model_comp_ref) {
			auto& model = model_comp_ref.get_model();
			if(!model.is_valid())
				return;

			const auto& world_transform = transform_comp_ref.get_transform();

			auto mesh = model.get_lod(0);
			if(!mesh)
				return;

			const auto& frustum = camera.get_frustum();
			const auto& bounds = mesh->get_bounds();

			// Test the bounding box of the mesh
			if(!math::frustum::test_obb(frustum, bounds, world_transform))
				return;

			auto entity_index = e.id().index();
			std::uint32_t rr = (entity_index)&0xff;
			std::uint32_t gg = (entity_index >> 8) & 0xff;
			std::uint32_t bb = (entity_index >> 16) & 0xff;
			math::vec4 color_id = {rr / 255.0f, gg / 255.0f, bb / 255.0f, 1.0f};

			model.render(pass.id, world_transform, true, true, true, 0, 0, _program.get(),
						 [&color_id](program& p) { p.set_uniform("u_id", &color_id); });
		});
	}

	// If the user previously clicked, and we're done reading data from GPU, look at ID buffer on CPU
	// Whatever mesh has the most pixels in the ID buffer is the one the user clicked on.
	if(!_reading && _start_readback)
	{
		render_pass pass("picking_buffer_blit");
		// Blit and read
		gfx::blit(pass.id, _blit_tex->handle, 0, 0, gfx::getTexture(_surface->handle));
		_reading = gfx::readTexture(_blit_tex->handle, _blit_data);
		_start_readback = false;
	}

	if(_reading && _reading <= render_frame)
	{
		_reading = 0;
		std::map<std::uint32_t, std::uint32_t> ids; // This contains all the IDs found in the buffer
		std::uint32_t max_amount = 0;
		for(std::uint8_t* x = _blit_data; x < _blit_data + tex_id_dim * tex_id_dim * 4;)
		{
			std::uint8_t rr = *x++;
			std::uint8_t gg = *x++;
			std::uint8_t bb = *x++;
			x++;
			/*std::uint8_t aa = *x++*/;

			const gfx::Caps* caps = gfx::getCaps();
			if(gfx::RendererType::Direct3D9 == caps->rendererType)
			{
				// Comes back as BGRA
				std::swap(rr, bb);
			}

			// Skip background
			if(0 == (rr | gg | bb))
			{
				continue;
			}

			std::uint32_t hash_key = static_cast<std::uint32_t>(rr + (gg << 8) + (bb << 16));
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
		if(max_amount)
		{
			for(auto& pair : ids)
			{
				if(pair.second == max_amount)
				{
					id_key = pair.first;
					if(ecs.valid_index(id_key))
					{
						auto eid = ecs.create_id(id_key);
						auto pickedEntity = ecs.get(eid);
						if(pickedEntity)
							es.select(pickedEntity);
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

bool picking_system::initialize()
{
	runtime::on_frame_render.connect(this, &picking_system::frame_render);
	// Set up ID buffer, which has a color target and depth buffer
	auto picking_rt =
		std::make_shared<texture>(tex_id_dim, tex_id_dim, false, 1, gfx::TextureFormat::RGBA8,
								  0 | BGFX_TEXTURE_RT | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT |
									  BGFX_TEXTURE_MIP_POINT | BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	auto picking_rt_depth =
		std::make_shared<texture>(tex_id_dim, tex_id_dim, false, 1, gfx::TextureFormat::D24S8,
								  0 | BGFX_TEXTURE_RT | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT |
									  BGFX_TEXTURE_MIP_POINT | BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	_surface =
		std::make_shared<frame_buffer>(std::vector<std::shared_ptr<texture>>{picking_rt, picking_rt_depth});

	// CPU texture for blitting to and reading ID buffer so we can see what was clicked on.
	// Impossible to read directly from a render target, you *must* blit to a CPU texture
	// first. Algorithm Overview: Render on GPU -> Blit to CPU texture -> Read from CPU
	// texture.
	_blit_tex = std::make_shared<texture>(
		tex_id_dim, tex_id_dim, false, 1, gfx::TextureFormat::RGBA8,
		0 | BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT |
			BGFX_TEXTURE_MIP_POINT | BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();

	auto vs_picking_id = am.load<shader>("editor_data:/shaders/vs_picking_id.sc");
	auto fs_picking_id = am.load<shader>("editor_data:/shaders/fs_picking_id.sc");

	ts.push_or_execute_on_main(
		[this](asset_handle<shader> vs, asset_handle<shader> fs) {
			_program = std::make_unique<program>(vs, fs);

		},
		vs_picking_id, fs_picking_id);

	return true;
}

void picking_system::dispose()
{
	runtime::on_frame_render.disconnect(this, &picking_system::frame_render);
}
}
