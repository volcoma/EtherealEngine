#include "shader.h"

Shader::~Shader()
{
	dispose();
}

void Shader::dispose()
{
	uniforms.clear();

	if (is_valid())
		gfx::destroyShader(handle);

	handle = { bgfx::invalidHandle };
}

bool Shader::is_valid() const
{
	return gfx::isValid(handle);
}

void Shader::populate(const gfx::Memory* _mem)
{
	dispose();

	handle = gfx::createShader(_mem);
}
