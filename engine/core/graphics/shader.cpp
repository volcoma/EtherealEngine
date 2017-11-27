#include "shader.h"

shader::shader(const gfx::Memory* mem)
{
	populate(mem);
}

shader::shader(gfx::ShaderHandle hndl)
{
	populate(hndl);
}

shader::shader(const gfx::EmbeddedShader* es, const char* name)
{
	populate(es, name);
}

shader::~shader()
{
	dispose();
}

void shader::dispose()
{
	uniforms.clear();

	if(is_valid())
		gfx::destroy(handle);

	handle = {bgfx::kInvalidHandle};
}

bool shader::is_valid() const
{
	return gfx::isValid(handle);
}

void shader::populate(const gfx::Memory* mem)
{
	populate(gfx::createShader(mem));
}

void shader::populate(const gfx::EmbeddedShader* es, const char* name)
{
	populate(gfx::createEmbeddedShader(es, gfx::getRendererType(), name));
}

void shader::populate(gfx::ShaderHandle hndl)
{
	dispose();

	handle = hndl;

	auto uniform_count = gfx::getShaderUniforms(handle);
	if (uniform_count > 0)
	{
		std::vector<gfx::UniformHandle> uniforms_handles(uniform_count);
		gfx::getShaderUniforms(handle, &uniforms_handles[0], uniform_count);
		uniforms.reserve(uniform_count);
		for (auto& uni : uniforms_handles)
		{
			auto uniform_var = std::make_shared<uniform>();
			uniform_var->populate(uni);
			uniforms.emplace_back(uniform_var);
		}
	}
}
