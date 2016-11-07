#pragma once
#include "FrameBuffer.h"
#include "Core/common/basetypes.hpp"
#include <vector>
#include <unordered_map>
#include <string>


struct TextureAttachment
{
	//!< Texture handle.
	std::shared_ptr<Texture> texture;
	//!< Mip level.
	uint16_t mip = 0;
	//!< Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

class RenderView
{
public:
	class RenderSurface
	{
		friend class RenderView;
	public:
		RenderSurface() = default;
		void setSize(const uSize& size);
		void setSize(gfx::BackbufferRatio::Enum ratio);
		uSize getSize() const;
		void populate(
			std::uint16_t _width,
			std::uint16_t _height,
			gfx::TextureFormat::Enum _format,
			std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
		);
		void populate(
			gfx::BackbufferRatio::Enum _ratio,
			gfx::TextureFormat::Enum _format,
			std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
		);
		void populate(const std::vector<std::shared_ptr<Texture>>& textures);
		void populate(const std::vector<TextureAttachment>& textures);
		void populate(
			std::uint8_t id,
			void* _nwh,
			std::uint16_t _width,
			std::uint16_t _height,
			gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
		);
		inline std::shared_ptr<FrameBuffer> getBuffer() const { return mBuffer; }
		inline const TextureAttachment& getAttachment(std::uint32_t index) const { return mTextures[index]; }

	private:
		gfx::BackbufferRatio::Enum mRatio = gfx::BackbufferRatio::Equal;
		uSize mSize = { 0, 0 };
		std::shared_ptr<FrameBuffer> mBuffer = std::make_shared<FrameBuffer>();
		std::vector<TextureAttachment> mTextures;
	};


	RenderView()
	{}

	~RenderView();

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear(std::uint16_t _flags
		, std::uint32_t _rgba = 0x000000ff
		, float _depth = 1.0f
		, std::uint8_t _stencil = 0) const;

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear();
	RenderSurface& getRenderSurface();
	RenderSurface& getRenderSurface(const std::string& id);
	//-----------------------------------------------------------------------------
	//  Name : getId ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint8_t getId() const;

	//-----------------------------------------------------------------------------
	//  Name : pushView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void pushView(std::shared_ptr<RenderView> view);

	//-----------------------------------------------------------------------------
	//  Name : popView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void popView();

	//-----------------------------------------------------------------------------
	//  Name : setView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void setView(std::shared_ptr<RenderView> view);

	//-----------------------------------------------------------------------------
	//  Name : clearStack ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void clearStack();

	//-----------------------------------------------------------------------------
	//  Name : getStack ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::vector<std::shared_ptr<RenderView>>& getStack();

	//-----------------------------------------------------------------------------
	//  Name : generateId ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------


private:
	static std::uint8_t generateId();
	std::uint8_t mId = generateId();
	std::unordered_map<std::string, RenderSurface>	mRenderSurfaces;
};

struct ScopedRenderView
{
	ScopedRenderView(std::shared_ptr<RenderView> renderView);
	~ScopedRenderView();
};
