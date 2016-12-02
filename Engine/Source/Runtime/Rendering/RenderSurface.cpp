#include "RenderSurface.h"
#include "Graphics/graphics.h"
#include <bitset>
static std::bitset<256> sViews(0);
static std::uint8_t sLastFreeIndex = 0;
static std::uint8_t sLastFreeWndIndex = 254;
static const std::uint8_t sMaxWindowViews = 32;

std::vector<std::shared_ptr<RenderSurface>>& getSurfaceStack()
{
	static std::vector<std::shared_ptr<RenderSurface>> surfaceStack;
	return surfaceStack;
}

void setViewRenderSurface(std::shared_ptr<RenderSurface> surface)
{
	const auto size = surface->getSize();
	const auto id = surface->getId();

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

	if (surface->getBufferRaw()->isValid())
	{
		gfx::setViewFrameBuffer(id, surface->getBufferRaw()->handle);
		gfx::touch(id);
	}
}


std::uint8_t getAvailableNormalId()
{
	// find the first unset bit
	std::uint8_t idx = sLastFreeIndex;

	while (idx < sViews.size() && sViews.test(idx))
		++idx;

	sViews[idx] = 1;
	return idx;
}

void releaseNormalId(std::uint8_t id)
{
	sViews[id] = 0;
	sLastFreeIndex = id;
}

std::uint8_t getAvailableWndId()
{
	// find the first unset bit
	std::uint8_t idx = sLastFreeWndIndex;

	while (idx < sViews.size() && sViews.test(idx))
		--idx;

	sViews[idx] = 1;
	return idx;
}

void releaseWndId(std::uint8_t id)
{
	sViews[id] = 0;
	sLastFreeWndIndex = id;
}
void releaseId(std::uint8_t id)
{
	if (id == 255)
		return;

	if (id > (sViews.size() - 1) - sMaxWindowViews)
		releaseWndId(id);
	else
		releaseNormalId(id);

}

std::uint8_t getAvailableId(bool window)
{
	if (window)
		return getAvailableWndId();
	else
		return getAvailableNormalId();
}

void RenderSurface::populate(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format, std::uint32_t _textureFlags /*= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP */)
{
	releaseId(mId);
	mId = getAvailableId(false);
	mBuffer->populate(_width, _height, _format, _textureFlags);
}

void RenderSurface::populate(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format, std::uint32_t _textureFlags /*= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP */)
{
	releaseId(mId);
	mId = getAvailableId(false);
	mBuffer->populate(_ratio, _format, _textureFlags);
}

void RenderSurface::populate(const std::vector<std::shared_ptr<Texture>>& textures)
{
	releaseId(mId);
	mId = getAvailableId(false);
	mBuffer->populate(textures);
}

void RenderSurface::populate(const std::vector<TextureAttachment>& textures)
{
	releaseId(mId);
	mId = getAvailableId(false);
	mBuffer->populate(textures);
}

void RenderSurface::populate(void* _nwh, std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _depthFormat /*= gfx::TextureFormat::UnknownDepth */)
{
	releaseId(mId);
	mId = getAvailableId(true);
	mBuffer->populate(_nwh, _width, _height, _depthFormat);
}

uSize RenderSurface::getSize() const
{
	return mBuffer->getSize();
}

RenderSurface::~RenderSurface()
{
	releaseId(mId);

	gfx::resetView(mId);
}


void RenderSurface::clear(std::uint16_t _flags, std::uint32_t _rgba /*= 0x000000ff */, float _depth /*= 1.0f */, std::uint8_t _stencil /*= 0*/) const
{
	gfx::setViewClear(mId
		, _flags
		, _rgba
		, _depth
		, _stencil
	);
}

void RenderSurface::clear() const
{
	gfx::setViewClear(mId
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL
		, 0x2D2D2DFF
		, 1.0f
		, 0
	);
}


void RenderSurface::pushSurface(std::shared_ptr<RenderSurface> surface)
{
	setViewRenderSurface(surface);
	getSurfaceStack().push_back(surface);
}

void RenderSurface::popSurface()
{
	auto& surfaceStack = getSurfaceStack();

	surfaceStack.pop_back();

	if (!surfaceStack.empty())
	{
		auto surface = surfaceStack.back();

		setViewRenderSurface(surface);
	}
}


void RenderSurface::clearStack()
{
	getSurfaceStack().clear();
}

