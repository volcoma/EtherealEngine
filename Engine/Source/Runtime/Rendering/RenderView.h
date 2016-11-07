#pragma once
#include "FrameBuffer.h"
#include "Core/common/basetypes.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct TextureAttachment
{
	/// Texture handle.
	std::shared_ptr<Texture> texture;
	/// Mip level.
	uint16_t mip = 0;
	/// Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

class RenderView
{
public:
	/// Internal definition
	class RenderSurface
	{
		friend class RenderView;
	public:
		//-----------------------------------------------------------------------------
		//  Name : setSize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void setSize(const uSize& size);

		//-----------------------------------------------------------------------------
		//  Name : setSize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void setSize(gfx::BackbufferRatio::Enum ratio);

		//-----------------------------------------------------------------------------
		//  Name : getSize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		uSize getSize() const;

		//-----------------------------------------------------------------------------
		//  Name : populate ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void populate(
			std::uint16_t _width,
			std::uint16_t _height,
			gfx::TextureFormat::Enum _format,
			std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
		);

		//-----------------------------------------------------------------------------
		//  Name : populate ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void populate(
			gfx::BackbufferRatio::Enum _ratio,
			gfx::TextureFormat::Enum _format,
			std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
		);

		//-----------------------------------------------------------------------------
		//  Name : populate ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void populate(const std::vector<std::shared_ptr<Texture>>& textures);

		//-----------------------------------------------------------------------------
		//  Name : populate ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void populate(const std::vector<TextureAttachment>& textures);

		//-----------------------------------------------------------------------------
		//  Name : populate ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void populate(
			std::uint8_t id,
			void* _nwh,
			std::uint16_t _width,
			std::uint16_t _height,
			gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
		);

		//-----------------------------------------------------------------------------
		//  Name : getBuffer ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline std::shared_ptr<FrameBuffer> getBuffer() const { return mBuffer; }

		//-----------------------------------------------------------------------------
		//  Name : getBufferRaw ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline FrameBuffer* getBufferRaw() const { return mBuffer.get(); }

		//-----------------------------------------------------------------------------
		//  Name : getAttachment ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline const TextureAttachment& getAttachment(std::uint32_t index) const { return mTextures[index]; }

	private:
		/// Back buffer ratio if any.
		gfx::BackbufferRatio::Enum mRatio = gfx::BackbufferRatio::Equal;
		/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
		uSize mSize = { 0, 0 };
		/// Texture attachments to the frame buffer
		std::vector<TextureAttachment> mTextures;
		/// Frame buffer
		std::shared_ptr<FrameBuffer> mBuffer = std::make_shared<FrameBuffer>();
	};

	RenderView() = default;

	//-----------------------------------------------------------------------------
	//  Name : ~RenderView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
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

	//-----------------------------------------------------------------------------
	//  Name : getRenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface& getRenderSurface();

	//-----------------------------------------------------------------------------
	//  Name : getRenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
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

private:
	//-----------------------------------------------------------------------------
	//  Name : generateId ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::uint8_t generateId();

	/// Id of the view.
	std::uint8_t mId = generateId();
	/// Render surfaces that this view owns.
	std::unordered_map<std::string, RenderSurface>	mRenderSurfaces;
};


struct RenderViewRAII
{
	//-----------------------------------------------------------------------------
	//  Name : RenderViewRAII ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderViewRAII(std::shared_ptr<RenderView> renderView);

	//-----------------------------------------------------------------------------
	//  Name : ~RenderViewRAII ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~RenderViewRAII();
};
