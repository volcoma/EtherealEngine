#include "shader.h"

namespace gfx
{
shader::shader(const memory_view* mem) : shader(create_shader(mem))
{
}

shader::shader(const embedded_shader* es, const char* name)
    : shader(create_embedded_shader(es, get_renderer_type(), name))
{
}

shader::shader(handle_type_t hndl)
{
    handle = hndl;

    auto uniform_count = get_shader_uniforms(handle);
    if(uniform_count > 0)
    {
        std::vector<uniform::handle_type_t> uniforms_handles(uniform_count);
        get_shader_uniforms(handle, &uniforms_handles[0], uniform_count);
        uniforms.reserve(uniform_count);
        for(auto& uni : uniforms_handles)
        {
            auto uniform_var = std::make_shared<uniform>(uni);
            uniforms.emplace_back(uniform_var);
        }
    }
}
} // namespace gfx
