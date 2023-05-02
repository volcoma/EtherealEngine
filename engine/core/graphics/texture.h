#pragma once

#include "../common/basetypes.hpp"
#include "handle_impl.h"
#include <memory>

namespace gfx
{
struct texture : public handle_impl<texture_handle>
{
    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture() = default;

    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture(const memory_view* _mem,
            std::uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
            std::uint8_t _skip = 0,
            texture_info* _info = nullptr);

    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture(std::uint16_t _width,
            std::uint16_t _height,
            bool _hasMips,
            std::uint16_t _numLayers,
            texture_format _format,
            std::uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
            const memory_view* _mem = nullptr);

    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture(backbuffer_ratio _ratio,
            bool _hasMips,
            std::uint16_t _numLayers,
            texture_format _format,
            std::uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture(std::uint16_t _width,
            std::uint16_t _height,
            std::uint16_t _depth,
            bool _hasMips,
            texture_format _format,
            std::uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
            const memory_view* _mem = nullptr);

    //-----------------------------------------------------------------------------
    //  Name : Texture ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    texture(std::uint16_t _size,
            bool _hasMips,
            std::uint16_t _numLayers,
            texture_format _format,
            std::uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
            const memory_view* _mem = nullptr);

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
    //  Name : is_render_target ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    bool is_render_target() const;

    /// Texture detail info.
    texture_info info;
    /// Creation flags.
    std::uint64_t flags = BGFX_TEXTURE_NONE;
    /// Back buffer ratio if any.
    backbuffer_ratio ratio = backbuffer_ratio::Count;
};
} // namespace gfx
