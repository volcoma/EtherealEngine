#pragma once

#include "uniform.h"
#include <memory>
#include <vector>

namespace gfx
{
struct shader : public handle_impl<shader_handle>
{
    shader() = default;
    shader(const memory_view* _mem);
    shader(const embedded_shader* _es, const char* name);
    shader(handle_type_t hndl);

    /// Uniforms for this shader
    std::vector<std::shared_ptr<uniform>> uniforms;
};
} // namespace gfx
