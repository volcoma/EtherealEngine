#include "Shader.h"

Shader::~Shader()
{
	dispose();
}

void Shader::dispose()
{
	if (isValid())
		gfx::destroyShader(handle);
}

bool Shader::isValid() const
{
	return gfx::isValid(handle);
}

void Shader::populate(const gfx::Memory* _mem)
{
	dispose();

	handle = gfx::createShader(_mem);
}
