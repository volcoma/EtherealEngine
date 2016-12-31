#include "render_pass.h"
#include "core/common/assert.hpp"
#include "graphics/graphics.h"
#include <bitset>

static std::uint8_t index = 0;
static std::uint8_t lastIndex = 0;

std::uint8_t generateId()
{
	Expects(index < 256);
	// find the first unset bit
	std::uint8_t idx = index++;

	lastIndex = idx;
	return idx;
}


RenderPass::RenderPass(const std::string& n)
{
	id = generateId();
}

void RenderPass::bind(FrameBuffer* fb) const
{
	Expects(fb != nullptr);

	const auto size = fb->getSize();

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
		, 0x2D2D2DFF
		, 1.0f
		, 0
	);
}

void RenderPass::reset()
{
	for (std::uint8_t i = 0; i < index; ++i)
	{
		gfx::resetView(i);
	}
	index = 0;
	lastIndex = 0;
}

std::uint8_t RenderPass::getPass()
{
	return lastIndex;
}
