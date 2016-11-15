#include "RenderSurface.h"
#include "Graphics/graphics.h"
#include <bitset>
static std::bitset<256> sViews(0);
static std::uint8_t sLastFreeIndex = 0;
static std::uint8_t sLastFreeWndIndex = 254;

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
	if (id < 200)
		releaseNormalId(id);
	else
		releaseWndId(id);

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
	mTextures.clear();
	mId = getAvailableId(false);
	mBuffer->populate(_width, _height, _format, _textureFlags);
	setSize({ _width, _height });
}

void RenderSurface::populate(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format, std::uint32_t _textureFlags /*= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP */)
{
	mTextures.clear();
	mId = getAvailableId(false);
	mBuffer->populate(_ratio, _format, _textureFlags);
	setSize(_ratio);
}

void RenderSurface::populate(const std::vector<std::shared_ptr<Texture>>& textures)
{
	std::vector<TextureAttachment> texDescs;
	texDescs.reserve(textures.size());
	for (auto& tex : textures)
	{
		TextureAttachment texDesc;
		texDesc.texture = tex;
		texDescs.push_back(texDesc);
	}

	populate(texDescs);
}

void RenderSurface::populate(const std::vector<TextureAttachment>& textures)
{
	mTextures.clear();
	mId = getAvailableId(false);

	std::vector<gfx::Attachment> buffer;
	buffer.reserve(textures.size());

	uSize size;
	gfx::BackbufferRatio::Enum ratio = gfx::BackbufferRatio::Enum::Count;
	for (auto& tex : textures)
	{
		ratio = tex.texture->ratio;
		size = { tex.texture->info.width, tex.texture->info.height };
		gfx::Attachment attachment;
		attachment.handle = tex.texture->handle;
		attachment.mip = tex.mip;
		attachment.layer = tex.layer;
		buffer.push_back(attachment);
	}
	mTextures = textures;
	mBuffer->populate(static_cast<std::uint8_t>(buffer.size()), &buffer[0], false);

	if (ratio == gfx::BackbufferRatio::Count)
		setSize(size);
	else
		setSize(ratio);
}

void RenderSurface::populate(void* _nwh, std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _depthFormat /*= gfx::TextureFormat::UnknownDepth */)
{
	mTextures.clear();
	mId = getAvailableId(true);

	mBuffer->populate(_nwh, _width, _height, _depthFormat);

	setSize({ _width, _height });
}

uSize RenderSurface::getSize() const
{
	if (mRatio == gfx::BackbufferRatio::Count)
	{
		return mSize;

	} // End if Absolute
	else
	{
		std::uint16_t width;
		std::uint16_t height;
		gfx::getSizeFromRatio(mRatio, width, height);
		uSize size =
		{
			static_cast<std::uint32_t>(width),
			static_cast<std::uint32_t>(height)
		};
		return size;

	} // End if Relative
}

RenderSurface::~RenderSurface()
{
	releaseId(mId);

	gfx::resetView(mId);
}


void RenderSurface::setSize(const uSize& size)
{
	mRatio = gfx::BackbufferRatio::Count;
	mSize = size;
}

void RenderSurface::setSize(gfx::BackbufferRatio::Enum ratio)
{
	mRatio = ratio;
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

