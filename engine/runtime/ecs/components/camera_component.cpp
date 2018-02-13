#include "camera_component.h"
#include "core/graphics/graphics.h"

camera_component::camera_component()
{
	camera_.set_viewport_size({640, 480});
}

camera_component::~camera_component()
{
}

void camera_component::update(const math::transform& t)
{
	// Release the unused fbos and textures
	render_view_.release_unused_resources();
	// First update so the camera can cache the previous matrices
	camera_.record_current_matrices();
	// Set new transform
	camera_.look_at(t.get_position(), t.get_position() + t.z_unit_axis(), t.y_unit_axis());
}

bool camera_component::get_hdr() const
{
	return hdr_;
}

void camera_component::set_hdr(bool hdr)
{
	hdr_ = hdr;
}

void camera_component::set_viewport_size(const usize32_t& size)
{
	camera_.set_viewport_size(size);
}

const usize32_t& camera_component::get_viewport_size() const
{
	return camera_.get_viewport_size();
}

float camera_component::get_ortho_size() const
{
	return camera_.get_ortho_size();
}

void camera_component::set_ortho_size(float size)
{
	camera_.set_orthographic_size(size);
}

float camera_component::get_ppu() const
{
	return camera_.get_ppu();
}

camera_component& camera_component::set_fov(float fovDegrees)
{
	camera_.set_fov(fovDegrees);

	return *this;
}

camera_component& camera_component::set_near_clip(float distance)
{
	camera_.set_near_clip(distance);

	return *this;
}
camera_component& camera_component::set_far_clip(float distance)
{
	camera_.set_far_clip(distance);

	return *this;
}

camera_component& camera_component::set_projection_mode(projection_mode mode)
{
	camera_.set_projection_mode(mode);

	return *this;
}

float camera_component::get_fov() const
{
	return camera_.get_fov();
}
float camera_component::get_near_clip() const
{
	return camera_.get_near_clip();
}
float camera_component::get_far_clip() const
{
	return camera_.get_far_clip();
}

projection_mode camera_component::get_projection_mode() const
{
	return camera_.get_projection_mode();
}
