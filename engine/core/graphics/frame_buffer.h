#pragma once

#include "graphics.h"
#include "texture.h"
#include <memory>
#include <vector>

namespace gfx
{
struct fbo_attachment
{
    /// Texture handle.
    std::shared_ptr<gfx::texture> texture;
    /// Mip level.
    std::uint16_t mip = 0;
    /// Cubemap side or depth layer/slice.
    std::uint16_t layer = 0;
};

struct frame_buffer : public handle_impl<frame_buffer_handle>
{
    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer() = default;

    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer(std::uint16_t _width,
                 std::uint16_t _height,
                 texture_format _format,
                 std::uint32_t _texture_flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer(backbuffer_ratio _ratio,
                 texture_format _format,
                 std::uint32_t _textureFlags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer(const std::vector<std::shared_ptr<texture>>& textures);

    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer(const std::vector<fbo_attachment>& textures);

    //-----------------------------------------------------------------------------
    //  Name : frame_buffer ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    frame_buffer(void* _nwh,
                 std::uint16_t _width,
                 std::uint16_t _height,
                 texture_format _format = texture_format::Count,
                 texture_format _depth_format = texture_format::Count);

    //-----------------------------------------------------------------------------
    //  Name : populate ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void populate(const std::vector<fbo_attachment>& textures);

    //-----------------------------------------------------------------------------
    //  Name : get_size ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    usize32_t get_size() const;

    //-----------------------------------------------------------------------------
    //  Name : get_attachment ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    const fbo_attachment& get_attachment(std::uint32_t index = 0) const;

    //-----------------------------------------------------------------------------
    //  Name : get_attachment ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::shared_ptr<gfx::texture>& get_texture(std::uint32_t index = 0) const;

    //-----------------------------------------------------------------------------
    //  Name : get_attachment_count ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    std::size_t get_attachment_count() const;

    /// Back buffer ratio if any.
    backbuffer_ratio bbratio_ = backbuffer_ratio::Equal;
    /// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
    usize32_t cached_size_ = {0, 0};
    /// Texture attachments to the frame buffer
    std::vector<fbo_attachment> textures_;
};
} // namespace gfx
