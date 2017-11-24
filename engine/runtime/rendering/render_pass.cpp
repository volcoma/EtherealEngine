#include "render_pass.h"
#include "core/common/assert.hpp"
#include "core/graphics/graphics.h"
#include <bitset>
#include <limits>
static std::uint8_t s_index = 0;
static std::uint8_t s_last_index = 0;

std::uint8_t generate_id()
{
	if(s_index == std::numeric_limits<decltype(s_index)>::max())
	{
		gfx::frame();
		s_index = 0;
	}
	// find the first unset bit
	std::uint8_t idx = s_index++;

	s_last_index = idx;
	return idx;
}

render_pass::render_pass(const std::string& n)
{
	id = generate_id();
	gfx::setViewName(id, n.c_str());
}
static urect viewport_rect;
void render_pass::bind(frame_buffer* fb) const
{
	expects(fb != nullptr);

	const auto size = fb->get_size();

	gfx::setViewRect(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(size.width),
					 std::uint16_t(size.height));

	gfx::setViewScissor(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(size.width),
						std::uint16_t(size.height));

	gfx::setViewFrameBuffer(id, fb->handle);
	gfx::touch(id);
    viewport_rect.left = 0;
    viewport_rect.top = 0;
    viewport_rect.right = size.width;
    viewport_rect.bottom = size.height;
}

void render_pass::bind() const
{
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    gfx::get_size_from_ratio(gfx::BackbufferRatio::Equal, width, height);
    gfx::setViewRect(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(width),
					 std::uint16_t(height));

	gfx::setViewScissor(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(width),
						std::uint16_t(height));

	
	gfx::touch(id);
}

void render_pass::clear(std::uint16_t _flags, std::uint32_t _rgba /*= 0x000000ff */, float _depth /*= 1.0f */,
						std::uint8_t _stencil /*= 0*/) const
{
	gfx::setViewClear(id, _flags, _rgba, _depth, _stencil);
}

void render_pass::clear() const
{
	gfx::setViewClear(id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0x000000FF, 1.0f, 0);
}

void render_pass::set_view_proj(const math::transform& v, const math::transform& p)
{
	gfx::setViewTransform(id, &v, &p);
}

void render_pass::set_view_proj_ortho_full(float depth)
{
	static const auto ortho_ = gfx::is_homogeneous_depth() ? math::orthoNO<float> : math::orthoZO<float>;
	static const math::transform p = ortho_(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, depth);

	gfx::setViewTransform(id, {}, &p);
}

void render_pass::reset()
{
	for(std::uint8_t i = 0; i < s_index; ++i)
	{
		gfx::resetView(i);
	}
	s_index = 0;
	s_last_index = 0;
}

std::uint8_t render_pass::get_pass()
{
    return s_last_index;
}

urect render_pass::get_pass_viewport()
{
    return viewport_rect;
}
