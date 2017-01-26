#include "camera_component.h"
#include "../../rendering/camera.h"
#include "../../rendering/render_pass.h"

CameraComponent::CameraComponent()
{
	_camera = std::make_unique<Camera>();
	_g_buffer = std::make_shared<FrameBuffer>();
	_light_buffer = std::make_shared<FrameBuffer>();
	_output_buffer = std::make_shared<FrameBuffer>();
	init({ 0, 0 });
}

CameraComponent::CameraComponent(const CameraComponent& cameraComponent)
{
	_camera = std::make_unique<Camera>(cameraComponent.get_camera());
	_hdr = cameraComponent._hdr;
	_g_buffer = std::make_shared<FrameBuffer>();
	_light_buffer = std::make_shared<FrameBuffer>();
	_output_buffer = std::make_shared<FrameBuffer>();
	init({ 0, 0 });
}

void CameraComponent::init(const uSize& size)
{
	std::uint32_t sampler_flags = 0
		| BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP 
		;

	auto surface_format = gfx::TextureFormat::RGBA8;
	auto depth_format = gfx::TextureFormat::D24;
	if (size.width == 0 && size.height == 0)
	{
		//create a depth buffer to share
		auto depth_buffer = std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, depth_format, sampler_flags);
		_g_buffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surface_format, sampler_flags),
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surface_format, sampler_flags),
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surface_format, sampler_flags),
				depth_buffer
			}
		);

		_light_buffer->populate(gfx::BackbufferRatio::Equal, gfx::TextureFormat::RGBA16F, sampler_flags);

		_output_buffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surface_format, sampler_flags),
				depth_buffer
			}
		);
	}
	else
	{
		//create a depth buffer to share
		auto depth_buffer = std::make_shared<Texture>(size.width, size.height, false, 1, depth_format, sampler_flags);
		_g_buffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(size.width, size.height, false, 1, surface_format, sampler_flags),
				std::make_shared<Texture>(size.width, size.height, false, 1, surface_format, sampler_flags),
				std::make_shared<Texture>(size.width, size.height, false, 1, surface_format, sampler_flags),
				depth_buffer
			}
		);

		_light_buffer->populate(size.width, size.height, gfx::TextureFormat::RGBA16F, sampler_flags);

		_output_buffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(size.width, size.height, false, 1, surface_format, sampler_flags),
				depth_buffer
			}
		);
	}

	update_projection_window();
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::update(const math::transform_t& t)
{
	// First update so the camera can cache the previous matrices
	_camera->record_current_matrices();

	// Set new transform
	_camera->look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());
	update_projection_window();
}

bool CameraComponent::get_hdr() const
{
	return _hdr;
}

void CameraComponent::set_hdr(bool hdr)
{
	_hdr = hdr;
}

void CameraComponent::set_viewport_size(const uSize& size)
{
	auto old_size = _output_buffer->get_size();
	if (size != old_size)
	{
		init(size);
	}

	update_projection_window();
}

const uSize& CameraComponent::get_viewport_size() const
{
	return _camera->get_viewport_size();
}

void CameraComponent::update_projection_window()
{
	auto size = _output_buffer->get_size();

	_camera->set_viewport_size(size);
}

float CameraComponent::get_ortho_size() const
{
	return _camera->get_ortho_size();
}

void CameraComponent::set_ortho_size(float size)
{
	_camera->set_orthographic_size(size);
}

float CameraComponent::get_ppu() const
{
	return _camera->get_ppu();
}

CameraComponent& CameraComponent::set_fov(float fovDegrees)
{
	_camera->set_fov(fovDegrees);

	return *this;
}

CameraComponent& CameraComponent::set_near_clip(float distance)
{
	_camera->set_near_clip(distance);

	return *this;
}
CameraComponent& CameraComponent::set_far_clip(float distance)
{
	_camera->set_far_clip(distance);

	return *this;
}

CameraComponent& CameraComponent::set_projection_mode(ProjectionMode mode)
{
	_camera->set_projection_mode(mode);

	update_projection_window();

	return *this;
}

float CameraComponent::get_fov() const
{
	return _camera->get_fov();
}
float CameraComponent::get_near_clip() const
{
	return _camera->get_near_clip();
}
float CameraComponent::get_far_clip() const
{
	return _camera->get_far_clip();
}

ProjectionMode CameraComponent::get_projection_mode() const
{
	return _camera->get_projection_mode();
}

std::shared_ptr<FrameBuffer> CameraComponent::get_output_buffer() const
{
	return _output_buffer;
}

std::shared_ptr<FrameBuffer> CameraComponent::get_g_buffer() const
{
	return _g_buffer;
}

std::shared_ptr<FrameBuffer> CameraComponent::get_light_buffer() const
{
	return _light_buffer;
}