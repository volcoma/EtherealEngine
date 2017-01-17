#include "picking_system.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/model.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/render_window.h"
#include "runtime/system/engine.h"
#include "runtime/rendering/renderer.h"
#include "runtime/input/input.h"
#include "../edit_state.h"

namespace editor
{
	void PickingSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto es = core::get_subsystem<EditState>();
		auto input = core::get_subsystem<runtime::Input>();
		auto engine = core::get_subsystem<runtime::Engine>();
		auto renderer = core::get_subsystem<runtime::Renderer>();
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

		const auto render_frame = renderer->get_render_frame();

		auto& editorCamera = es->camera;
		if (imguizmo::is_over() && es->selection_data.object)
			return;

		if (!editorCamera || !editorCamera.has_component<CameraComponent>())
			return;

		auto cameraComponentRef = editorCamera.component<CameraComponent>();
		auto cameraComponent = cameraComponentRef.lock();
		auto transformComponentRef = editorCamera.component<TransformComponent>();
		auto transformComponent = transformComponentRef.lock();
		auto& camera = cameraComponent->get_camera();
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		auto nearClip = camera.get_near_clip();
		auto farClip = camera.get_far_clip();
		auto viewProj = proj * view;
		auto invViewProj = math::inverse(viewProj);
		const auto& mouse_pos = input->get_current_cursor_position();
		const auto& frustum = camera.get_frustum();
		math::vec2 cursor_pos = math::vec2{ mouse_pos.x, mouse_pos.y };
		math::vec3 pickEye;
		math::vec3 pickAt;
		math::vec3 pickUp = { 0.0f, 1.0f, 0.0f };

		if (!camera.viewport_to_world(cursor_pos, frustum.planes[math::VolumePlane::Side::Near], pickEye, true))
			return;

		if (!camera.viewport_to_world(cursor_pos, frustum.planes[math::VolumePlane::Side::Far], pickAt, true))
			return;

		if (input->is_mouse_button_pressed(sf::Mouse::Left))
		{
			_reading = 0;
			_start_readback = true;
			auto pickView = math::lookAt(pickEye, pickAt, pickUp);
			auto pickProj = math::perspective(math::radians(1.0f), 1.0f, nearClip, farClip);

			RenderPass pass("PickingBufferFill");
			pass.bind(_surface.get());
			// ID buffer clears to black, which represents clicking on nothing (background)
			pass.clear(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
				, 0x000000ff
				, 1.0f
				, 0);


			// View rect and transforms for picking pass
			gfx::setViewTransform(pass.id, &pickView, &pickProj);

			ecs->each<TransformComponent, ModelComponent>([this, &pass, &camera](
				runtime::Entity e,
				TransformComponent& transformComponent,
				ModelComponent& modelComponent
				)
			{
				auto& model = modelComponent.get_model();
				if (!model.is_valid())
					return;

				const auto& worldTransform = transformComponent.get_transform();

				auto hMesh = model.get_lod(0);
				if (!hMesh)
					return;

				const auto& frustum = camera.get_frustum();
				const auto& bounds = hMesh->aabb;

				// Test the bounding box of the mesh
				if (!math::frustum::testOBB(frustum, bounds, worldTransform))
					return;

				auto entityIndex = e.id().index();
				std::uint32_t rr = (entityIndex) & 0xff;
				std::uint32_t gg = (entityIndex >> 8) & 0xff;
				std::uint32_t bb = (entityIndex >> 16) & 0xff;
				math::vec4 colorId =
				{
					rr / 255.0f,
					gg / 255.0f,
					bb / 255.0f,
					1.0f
				};

				model.render_lod(
					pass.id, 
					worldTransform,
					true,
					true,
					true,
					0,
					0,
					_program.get(),
					[&colorId](Program& program)
				{
					program.set_uniform("u_id", &colorId);
				});
			});
		}

		// If the user previously clicked, and we're done reading data from GPU, look at ID buffer on CPU
		// Whatever mesh has the most pixels in the ID buffer is the one the user clicked on.
		if (!_reading && _start_readback)
		{
			RenderPass pass("PickingBufferBlit");
			// Blit and read
			gfx::blit(pass.id, _blit_tex->handle, 0, 0, gfx::getTexture(_surface->handle));
			_reading = gfx::readTexture(_blit_tex->handle, _blit_data);
			_start_readback = false;
		}

		if (_reading && _reading <= render_frame)
		{
			_reading = 0;
			std::map<std::uint32_t, std::uint32_t> ids;  // This contains all the IDs found in the buffer
			std::uint32_t maxAmount = 0;
			for (std::uint8_t* x = _blit_data; x < _blit_data + _id_dimensions * _id_dimensions * 4;)
			{
				std::uint8_t rr = *x++;
				std::uint8_t gg = *x++;
				std::uint8_t bb = *x++;
				x++;
				/*std::uint8_t aa = *x++*/;

				const gfx::Caps* caps = gfx::getCaps();
				if (gfx::RendererType::Direct3D9 == caps->rendererType)
				{
					// Comes back as BGRA
					std::swap(rr, bb);
				}

				// Skip background
				if (0 == (rr | gg | bb))
				{
					continue;
				}

				std::uint32_t hashKey = rr + (gg << 8) + (bb << 16);
				std::uint32_t amount = 1;
				auto mapIter = ids.find(hashKey);
				if (mapIter != ids.end())
				{
					amount = mapIter->second + 1;
				}

				// Amount of times this ID (color) has been clicked on in buffer
				ids[hashKey] = amount;
				maxAmount = maxAmount > amount ? maxAmount : amount;
			}

			std::uint32_t idKey = 0;
			if (maxAmount)
			{
				for (auto& pair : ids)
				{
					if (pair.second == maxAmount)
					{
						idKey = pair.first;
						if (ecs->valid_index(idKey))
						{
							auto eid = ecs->create_id(idKey);
							auto pickedEntity = ecs->get(eid);
							if (pickedEntity)
								es->select(pickedEntity);
						}
						break;
					}
				}

			}
			else
			{
				es->unselect();
			}
		}
	}

	bool PickingSystem::initialize()
	{
		runtime::on_frame_render.connect(this, &PickingSystem::frame_render);
		// Set up ID buffer, which has a color target and depth buffer
		auto pickingRT = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::RGBA8, 0
			| BGFX_TEXTURE_RT
			| BGFX_TEXTURE_MIN_POINT
			| BGFX_TEXTURE_MAG_POINT
			| BGFX_TEXTURE_MIP_POINT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP
			);

		auto pickingRTDepth = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::D24S8, 0
			| BGFX_TEXTURE_RT
			| BGFX_TEXTURE_MIN_POINT
			| BGFX_TEXTURE_MAG_POINT
			| BGFX_TEXTURE_MIP_POINT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP
			);

		_surface = std::make_shared<FrameBuffer>
			(
				std::vector<std::shared_ptr<Texture>>
		{
			pickingRT,
				pickingRTDepth
		}
		);

		// CPU texture for blitting to and reading ID buffer so we can see what was clicked on.
		// Impossible to read directly from a render target, you *must* blit to a CPU texture
		// first. Algorithm Overview: Render on GPU -> Blit to CPU texture -> Read from CPU
		// texture.
		_blit_tex = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::RGBA8, 0
			| BGFX_TEXTURE_BLIT_DST
			| BGFX_TEXTURE_READ_BACK
			| BGFX_TEXTURE_MIN_POINT
			| BGFX_TEXTURE_MAG_POINT
			| BGFX_TEXTURE_MIP_POINT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP
			);

		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<Shader>("editor_data:/shaders/vs_picking_id", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("editor_data:/shaders/fs_picking_id", false)
				.then([this, vs](auto fs)
			{
				_program = std::make_unique<Program>(vs, fs);
			});
		});
		return true;
	}

	void PickingSystem::dispose()
	{
		runtime::on_frame_render.disconnect(this, &PickingSystem::frame_render);
	}

}