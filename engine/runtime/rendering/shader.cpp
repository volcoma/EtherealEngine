#include "shader.h"

shader::~shader()
{
	dispose();
}

void shader::dispose()
{
	uniforms.clear();

	if(is_valid())
		gfx::destroyShader(handle);

	handle = {bgfx::kInvalidHandle};
}

bool shader::is_valid() const
{
	return gfx::isValid(handle);
}

void shader::populate(const gfx::Memory* _mem)
{
	dispose();

	handle = gfx::createShader(_mem);
}
