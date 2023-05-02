#pragma once

#include "../common/basetypes.hpp"
#include "../common/hash.hpp"
#include "frame_buffer.h"
#include "render_view_keys.h"
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

namespace gfx
{

class render_view
{
public:
    std::shared_ptr<texture> get_texture(const std::string& id,
                                         std::uint16_t _width,
                                         std::uint16_t _height,
                                         bool _hasMips,
                                         std::uint16_t _numLayers,
                                         texture_format _format,
                                         std::uint64_t _flags = get_default_rt_sampler_flags(),
                                         const memory_view* _mem = nullptr);

    std::shared_ptr<texture> get_texture(const std::string& id,
                                         backbuffer_ratio _ratio,
                                         bool _hasMips,
                                         std::uint16_t _numLayers,
                                         texture_format _format,
                                         std::uint64_t _flags = get_default_rt_sampler_flags());

    std::shared_ptr<texture> get_texture(const std::string& id,
                                         std::uint16_t _width,
                                         std::uint16_t _height,
                                         std::uint16_t _depth,
                                         bool _hasMips,
                                         texture_format _format,
                                         std::uint64_t _flags = get_default_rt_sampler_flags(),
                                         const memory_view* _mem = nullptr);

    std::shared_ptr<texture> get_texture(const std::string& id,
                                         std::uint16_t _size,
                                         bool _hasMips,
                                         std::uint16_t _numLayers,
                                         texture_format _format,
                                         std::uint64_t _flags = get_default_rt_sampler_flags(),
                                         const memory_view* _mem = nullptr);

    std::shared_ptr<frame_buffer> get_fbo(const std::string& id,
                                          const std::vector<std::shared_ptr<texture>>& bind_textures);

    std::shared_ptr<texture> get_depth_stencil_buffer(const usize32_t& viewport_size);
    std::shared_ptr<texture> get_depth_buffer(const usize32_t& viewport_size);
    std::shared_ptr<texture> get_depth_buffer(const usize32_t& viewport_size, size_t i);

    std::shared_ptr<texture> get_output_buffer(const usize32_t& viewport_size);
    std::shared_ptr<frame_buffer> get_output_fbo(const usize32_t& viewport_size);
    std::shared_ptr<frame_buffer> get_g_buffer_fbo(const usize32_t& viewport_size);

    void release_unused_resources();

private:
    std::unordered_map<texture_key, std::pair<std::shared_ptr<texture>, bool>> textures_;
    std::unordered_map<fbo_key, std::pair<std::shared_ptr<frame_buffer>, bool>> fbos_;
};
} // namespace gfx
