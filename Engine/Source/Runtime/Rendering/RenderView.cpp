#include "RenderView.h"
#include "Graphics/graphics.h"
#include <bitset>
static std::bitset<256> views(0);
static std::uint8_t lastFreeIndex = 0;


std::uint8_t getAvailableId()
{
	// find the first unset bit
	std::uint8_t idx = lastFreeIndex;

	while (idx < views.size() && views.test(idx))
		++idx;

	views[idx] = 1;
	return idx;
}


void releaseId(std::uint8_t id)
{
	views[id] = 0;
	lastFreeIndex = id;
}

RenderView::~RenderView()
{
	mRenderSurfaces.clear();
	gfx::setViewFrameBuffer(mId, {gfx::invalidHandle});
	releaseId(mId);
}

void RenderView::clear(std::uint16_t _flags, std::uint32_t _rgba /*= 0x000000ff */, float _depth /*= 1.0f */, std::uint8_t _stencil /*= 0*/) const
{
	gfx::setViewClear(mId
		, _flags
		, _rgba
		, _depth
		, _stencil
	);
}

void RenderView::clear()
{
	gfx::setViewClear(mId
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x2D2D2DFF
		, 1.0f
		, 0
	);
}


std::uint8_t RenderView::getId() const
{
	return mId;
}

RenderView::RenderSurface& RenderView::getRenderSurface()
{
	return getRenderSurface("");
}
RenderView::RenderSurface& RenderView::getRenderSurface(const std::string& id)
{
	auto& surface = mRenderSurfaces[id];
	return surface;
}

void RenderView::pushView(std::shared_ptr<RenderView> view)
{
	setView(view);

	getStack().push_back(view);
}

void RenderView::popView()
{
	getStack().pop_back();

	if (!getStack().empty())
	{
		auto view = getStack().back();

		setView(view);
	}
}

void RenderView::setView(std::shared_ptr<RenderView> view)
{
	// Set view default view port.
	auto size = view->getRenderSurface().getSize();

	gfx::setViewRect(
		view->getId(),
		std::uint16_t(0),
		std::uint16_t(0),
		std::uint16_t(size.width),
		std::uint16_t(size.height)
	);

	gfx::setViewScissor(
		view->getId(),
		std::uint16_t(0),
		std::uint16_t(0),
		std::uint16_t(size.width),
		std::uint16_t(size.height)
	);
	auto& surface = view->getRenderSurface();
	if (surface.getBuffer()->isValid())
	{
		gfx::setViewFrameBuffer(view->mId, surface.getBuffer()->handle);
		gfx::touch(view->mId);
	}
}

void RenderView::clearStack()
{
	getStack() = std::vector<std::shared_ptr<RenderView>>();
}

std::vector<std::shared_ptr<RenderView>>& RenderView::getStack()
{
	static std::vector<std::shared_ptr<RenderView>> viewStack;
	return viewStack;
}

std::uint8_t RenderView::generateId()
{
	return getAvailableId();
}



void RenderView::RenderSurface::populate(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format, std::uint32_t _textureFlags /*= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP */)
{
	mBuffer->populate(_width, _height, _format, _textureFlags);
	setSize({ _width, _height });
}

void RenderView::RenderSurface::populate(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format, std::uint32_t _textureFlags /*= BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP */)
{
	mBuffer->populate(_ratio, _format, _textureFlags);
	setSize(_ratio);
}

void RenderView::RenderSurface::populate(const std::vector<std::shared_ptr<Texture>>& textures)
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

void RenderView::RenderSurface::populate(const std::vector<TextureAttachment>& textures)
{

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

void RenderView::RenderSurface::populate(std::uint8_t id, void* _nwh, std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _depthFormat /*= gfx::TextureFormat::UnknownDepth */)
{
	if (id != 0)
		mBuffer->populate(_nwh, _width, _height, _depthFormat);
	setSize({ _width, _height });
}

uSize RenderView::RenderSurface::getSize() const
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

void RenderView::RenderSurface::setSize(const uSize& size)
{
	mRatio = gfx::BackbufferRatio::Count;
	mSize = size;
}

void RenderView::RenderSurface::setSize(gfx::BackbufferRatio::Enum ratio)
{
	mRatio = ratio;
}
ScopedRenderView::ScopedRenderView(std::shared_ptr<RenderView> renderView)
{
	RenderView::pushView(renderView);
}

ScopedRenderView::~ScopedRenderView()
{
	RenderView::popView();
}
