#include "reflection_probe_component.h"

ReflectionProbeComponent::ReflectionProbeComponent()
{
}

ReflectionProbeComponent::ReflectionProbeComponent(const ReflectionProbeComponent& lightComponent)
{
	_probe = lightComponent._probe;
}

ReflectionProbeComponent::~ReflectionProbeComponent()
{

}

int ReflectionProbeComponent::compute_projected_sphere_rect(iRect& rect, const math::vec3& position, const math::transform_t& view, const math::transform_t& proj)
{
	if (_probe.probe_type == ProbeType::Sphere)
	{
		return math::compute_projected_sphere_rect(
			rect.left,
			rect.right,
			rect.top,
			rect.bottom,
			position,
			_probe.sphere_data.range,
			view,
			proj);
	}
	else if (_probe.probe_type == ProbeType::Box)
	{
		float w2 = math::pow(_probe.box_data.extents.x * 2.0f, 2.0f);
		float h2 = math::pow(_probe.box_data.extents.y * 2.0f, 2.0f);
		float l2 = math::pow(_probe.box_data.extents.z * 2.0f, 2.0f);
		float d2 = w2 + h2 + l2;
		float d = math::sqrt(d2);

		return math::compute_projected_sphere_rect(
			rect.left,
			rect.right,
			rect.top,
			rect.bottom,
			position,
			d,
			view,
			proj);
	}
	else
	{
		return 1;
	}
}

std::shared_ptr<Texture> ReflectionProbeComponent::get_cubemap()
{
	static auto buffer_format = gfx::get_best_format(
		BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER |
		BGFX_CAPS_FORMAT_TEXTURE_CUBE |
		BGFX_CAPS_FORMAT_TEXTURE_MIP_AUTOGEN,
		gfx::FormatSearchFlags::FourChannels |
		gfx::FormatSearchFlags::RequireAlpha);

	static auto flags = gfx::get_default_rt_sampler_flags() | BGFX_TEXTURE_BLIT_DST;

	std::uint16_t size = 256;
	return _render_view[0].get_texture("CUBEMAP", size, true, 1, buffer_format, flags);
}


std::shared_ptr<FrameBuffer> ReflectionProbeComponent::get_cubemap_fbo()
{
	return _render_view[0].get_fbo("CUBEMAP", {get_cubemap()});
}

void ReflectionProbeComponent::set_probe(const ReflectionProbe& probe)
{
	if (probe == _probe)
		return;

	touch();

	_probe = probe;
}
