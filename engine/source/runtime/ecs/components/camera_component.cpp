#include "camera_component.h"
#include "../../rendering/camera.h"
#include "graphics/graphics.h"

CameraComponent::CameraComponent()
{
	_camera = std::make_unique<Camera>();
	auto stats = gfx::getStats();
	_camera->set_viewport_size({ stats->width, stats->height });
}

CameraComponent::CameraComponent(const CameraComponent& cameraComponent)
{
	_camera = std::make_unique<Camera>(cameraComponent.get_camera());
	_hdr = cameraComponent._hdr;
	auto stats = gfx::getStats();
	_camera->set_viewport_size({ stats->width, stats->height });
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::update(const math::transform_t& t)
{
	// Release the unused fbos and textures
	_render_view.release_unused_resources();
	// First update so the camera can cache the previous matrices
	_camera->record_current_matrices();
	// Set new transform
	_camera->look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());

	const auto& viewport_size = _camera->get_viewport_size();
	if (viewport_size.width == 0 && viewport_size.height == 0)
	{
		auto stats = gfx::getStats();
		_camera->set_viewport_size({ stats->width, stats->height });
	}
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
	_camera->set_viewport_size(size);
}

const uSize& CameraComponent::get_viewport_size() const
{
	return _camera->get_viewport_size();
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
