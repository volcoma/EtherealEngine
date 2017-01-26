#include "render_pass.h"
#include "core/common/assert.hpp"
#include "graphics/graphics.h"
#include <bitset>

static std::uint8_t index = 0;
static std::uint8_t last_index = 0;

std::uint8_t generate_id()
{
	Expects(index < 256);
	// find the first unset bit
	std::uint8_t idx = index++;

	last_index = idx;
	return idx;
}


RenderPass::RenderPass(const std::string& n)
{
	id = generate_id();
	gfx::setViewName(id, n.c_str());
}

void RenderPass::bind(FrameBuffer* fb) const
{
	Expects(fb != nullptr);

	const auto size = fb->get_size();

	gfx::setViewRect(
		id,
		std::uint16_t(0),
		std::uint16_t(0),
		std::uint16_t(size.width),
		std::uint16_t(size.height)
	);

	gfx::setViewScissor(
		id,
		std::uint16_t(0),
		std::uint16_t(0),
		std::uint16_t(size.width),
		std::uint16_t(size.height)
	);

	
	gfx::setViewFrameBuffer(id, fb->handle);
	gfx::touch(id);
	
}

void RenderPass::clear(std::uint16_t _flags, std::uint32_t _rgba /*= 0x000000ff */, float _depth /*= 1.0f */, std::uint8_t _stencil /*= 0*/) const
{
	gfx::setViewClear(id
		, _flags
		, _rgba
		, _depth
		, _stencil
	);
}

void RenderPass::clear() const
{
	gfx::setViewClear(id
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL
		, 0x000000FF
		, 1.0f
		, 0
	);
}

void RenderPass::set_view_proj(const math::transform_t& v, const math::transform_t& p)
{
	gfx::setViewTransform(id, &v, &p);
}

void RenderPass::set_view_proj_ortho_full()
{
	static const math::transform_t p = math::ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f, gfx::getCaps()->homogeneousDepth);
	gfx::setViewTransform(id, {}, &p);
}

void RenderPass::reset()
{
	for (std::uint8_t i = 0; i < index; ++i)
	{
		gfx::resetView(i);
	}
	index = 0;
	last_index = 0;
}

std::uint8_t RenderPass::get_pass()
{
	return last_index;
}
