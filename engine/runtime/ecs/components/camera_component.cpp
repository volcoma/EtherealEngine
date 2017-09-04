#include "camera_component.h"
#include "core/graphics/graphics.h"

camera_component::camera_component()
{
	auto stats = gfx::getStats();
	_camera.set_viewport_size({stats->width, stats->height});
}

camera_component::~camera_component()
{
}

void camera_component::update(const math::transform& t)
{
	// Release the unused fbos and textures
	_render_view.release_unused_resources();
	// First update so the camera can cache the previous matrices
	_camera.record_current_matrices();
	// Set new transform
	_camera.look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());

	const auto& viewport_size = _camera.get_viewport_size();
	if(viewport_size.width == 0 && viewport_size.height == 0)
	{
		auto stats = gfx::getStats();
		_camera.set_viewport_size({stats->width, stats->height});
	}
}

bool camera_component::get_hdr() const
{
	return _hdr;
}

void camera_component::set_hdr(bool hdr)
{
	_hdr = hdr;
}

void camera_component::set_viewport_size(const usize& size)
{
	_camera.set_viewport_size(size);
}

const usize& camera_component::get_viewport_size() const
{
	return _camera.get_viewport_size();
}

float camera_component::get_ortho_size() const
{
	return _camera.get_ortho_size();
}

void camera_component::set_ortho_size(float size)
{
	_camera.set_orthographic_size(size);
}

float camera_component::get_ppu() const
{
	return _camera.get_ppu();
}

camera_component& camera_component::set_fov(float fovDegrees)
{
	_camera.set_fov(fovDegrees);

	return *this;
}

camera_component& camera_component::set_near_clip(float distance)
{
	_camera.set_near_clip(distance);

	return *this;
}
camera_component& camera_component::set_far_clip(float distance)
{
	_camera.set_far_clip(distance);

	return *this;
}

camera_component& camera_component::set_projection_mode(projection_mode mode)
{
	_camera.set_projection_mode(mode);

	return *this;
}

float camera_component::get_fov() const
{
	return _camera.get_fov();
}
float camera_component::get_near_clip() const
{
	return _camera.get_near_clip();
}
float camera_component::get_far_clip() const
{
	return _camera.get_far_clip();
}

projection_mode camera_component::get_projection_mode() const
{
	return _camera.get_projection_mode();
}
