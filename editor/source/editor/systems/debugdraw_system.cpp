#include "debugdraw_system.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/model.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/program.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/debugdraw/debugdraw.h"
#include "runtime/assets/asset_manager.h"
#include "../edit_state.h"
#include "runtime/system/engine.h"

namespace editor
{
	void DebugDrawSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto es = core::get_subsystem<EditState>();

		auto& editorCamera = es->camera;
		auto& selected = es->selection_data.object;
		if (!editorCamera ||
			!editorCamera.has_component<CameraComponent>())
			return;

		const auto cameraComponentRef = editorCamera.component<CameraComponent>();
		const auto cameraComponent = cameraComponentRef.lock();
		const auto surface = cameraComponent->get_output_buffer();
		auto& camera = cameraComponent->get_camera();
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		const auto cameraPos = camera.get_position();

		RenderPass pass("DebugDrawPass");
		pass.bind(surface.get());
		gfx::setViewTransform(pass.id, &view, &proj);
		ddRAII dd(pass.id);
		const std::uint32_t colorGrid = 0xff606060;
		auto drawGrid = [](std::uint32_t gridColor, float height, float heightIntervals, std::uint32_t gridSize, std::uint32_t sizeIntervals, std::uint32_t iteration, std::uint32_t maxIterations)
		{
			std::uint32_t detailGridColor = gridColor;

			bool shouldRender = true;
			if (iteration + 1 != maxIterations)
			{
				const auto iterationHeight = heightIntervals * float(iteration + 1);
				const float factor = math::clamp(height, 0.0f, iterationHeight) / iterationHeight;
				std::uint32_t r = (gridColor) & 0xff;
				std::uint32_t g = (gridColor >> 8) & 0xff;
				std::uint32_t b = (gridColor >> 16) & 0xff;
				std::uint32_t a = (gridColor >> 24) & 0xff;
				a = static_cast<std::uint32_t>(math::lerp(255.0f, 0.0f, factor));
				if (a < 10)
					shouldRender = false;

				detailGridColor = r + (g << 8) + (b << 16) + (a << 24);
			}


			if (shouldRender)
			{
				auto step = (sizeIntervals * iteration);
				step = step ? step : 1;
				ddPush();
				ddSetState(true, false, true);
				ddSetColor(detailGridColor);
				math::vec3 center = { 0.0f, 0.0f, 0.0f };
				math::vec3 normal = { 0.0f, 1.0f, 0.0f };
				ddDrawGrid(&normal, &center, gridSize / step, float(step));
				ddPop();
			}

		};

		if (es->show_grid)
		{
			static const auto farClip = 200;
			static const auto height = 40.0f;
			static const auto divison = 10;
			const auto iterations = math::power_of_n_round_down(farClip, divison);
			for (std::uint32_t i = 0; i < iterations; ++i)
			{
				drawGrid(colorGrid, math::abs(cameraPos.y), height, farClip, divison, i, iterations);
			}
		}


		if (!selected || !selected.is_type<runtime::Entity>())
			return;

		auto selectedEntity = selected.get_value<runtime::Entity>();

		if (!selectedEntity ||
			!selectedEntity.has_component<TransformComponent>())
			return;


		auto& transformComponent = *selectedEntity.component<TransformComponent>().lock();
		const auto& worldTransform = transformComponent.get_transform();

		if (selectedEntity.has_component<CameraComponent>() && selectedEntity != editorCamera)
		{
			auto& cameraComponent = *selectedEntity.component<CameraComponent>().lock();
			auto& selectedCamera = cameraComponent.get_camera();
			const auto viewProj = selectedCamera.get_projection() * selectedCamera.get_view();
			ddPush();
			ddSetColor(0xffffffff);
			ddSetTransform(nullptr);
			ddDrawFrustum(&viewProj);
			ddPop();
		}

		if (selectedEntity.has_component<LightComponent>())
		{
			auto& lightComponent = *selectedEntity.component<LightComponent>().lock();
			auto& light = lightComponent.get_light();
			if (light.light_type == LightType::Spot)
			{
				auto adjacent = light.spot_data.range;
				{
					auto tanAngle = math::tan(math::radians(light.spot_data.spot_outer_angle));
					// oposite = tan * adjacent
					auto oposite = tanAngle * adjacent;
					ddPush();
					ddSetColor(0xff00ff00);
					ddSetWireframe(true);
					math::vec3 from = transformComponent.get_position();
					math::vec3 to = from + transformComponent.get_z_axis() * adjacent;
					ddDrawCone(&to, &from, oposite);
					ddPop();
				}
				{
					auto tanAngle = math::tan(math::radians(light.spot_data.spot_inner_angle));
					// oposite = tan * adjacent
					auto oposite = tanAngle * adjacent;
					ddPush();
					ddSetColor(0xff00ffff);
					ddSetWireframe(true);
					math::vec3 from = transformComponent.get_position();
					math::vec3 to = from + transformComponent.get_z_axis() * adjacent;
					ddDrawCone(&to, &from, oposite);
					ddPop();
				}
			}
			else if (light.light_type == LightType::Point)
			{
				auto radius = light.point_data.range;
				ddPush();
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				math::vec3 center = transformComponent.get_position();
				ddDrawCircle(Axis::X, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Y, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Z, center.x, center.y, center.z, radius);
				ddPop();
			}
			else if (light.light_type == LightType::Directional)
			{
				ddPush();
				ddSetLod(UINT8_MAX);
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				math::vec3 from1 = transformComponent.get_position();
				math::vec3 to1 = from1 + transformComponent.get_z_axis() * 2.0f;
				ddDrawCylinder(&from1, &to1, 0.1f);
				math::vec3 from2 = to1;
				math::vec3 to2 = from2 + transformComponent.get_z_axis() * 1.5f;;
				ddDrawCone(&from2, &to2, 0.5f);
				ddPop();
			}
		}

		if (selectedEntity.has_component<ModelComponent>())
		{
			const auto& modelComponent = *selectedEntity.component<ModelComponent>().lock();
			const auto& model = modelComponent.get_model();
			if (!model.is_valid())
				return;

			const auto hMesh = model.get_lod(0);
			if (!hMesh)
				return;
			const auto& frustum = camera.get_frustum();
			const auto& bounds = hMesh->aabb;

			// Test the bounding box of the mesh
			if (math::frustum::testOBB(frustum, bounds, worldTransform))
			{
				if (es->wireframe_selection)
				{
					const float u_params[8] =
					{
						1.0f, 1.0f, 0.0f, 0.5f, //r,g,b,a
						1.0f, 0.0f, 0.0f, 0.0f  //thickness, unused, unused, unused
					};
					if (!_program)
						return;

					model.render_lod(
						pass.id,
						worldTransform,
						false,
						false,
						false,
						BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA),
						0,
						_program.get(), [&u_params](Program& program)
					{
						program.set_uniform("u_params", &u_params, 2);
					});
				}
				else
				{
					ddPush();
					ddSetColor(0xff00ff00);
					ddSetTransform(&worldTransform);
					Aabb aabb;
					aabb.m_min[0] = bounds.min.x;
					aabb.m_min[1] = bounds.min.y;
					aabb.m_min[2] = bounds.min.z;
					aabb.m_max[0] = bounds.max.x;
					aabb.m_max[1] = bounds.max.y;
					aabb.m_max[2] = bounds.max.z;
					ddDraw(aabb);
					ddPop();
				}
			}
		}
	}


	bool DebugDrawSystem::initialize()
	{
		runtime::on_frame_render.connect(this, &DebugDrawSystem::frame_render);

		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<Shader>("editor_data:/shaders/vs_wf_wireframe", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("editor_data:/shaders/fs_wf_wireframe", false)
				.then([this, vs](auto fs)
			{
				_program = std::make_unique<Program>(vs, fs);
			});
		});

		ddInit();
		return true;
	}

	void DebugDrawSystem::dispose()
	{
		ddShutdown();
		runtime::on_frame_render.disconnect(this, &DebugDrawSystem::frame_render);
	}

}