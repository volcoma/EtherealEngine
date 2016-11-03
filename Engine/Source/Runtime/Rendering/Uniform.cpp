#include "Uniform.h"

Uniform::~Uniform()
{
	dispose();
}

void Uniform::dispose()
{
	if (isValid())
		gfx::destroyUniform(handle);
}

bool Uniform::isValid() const
{
	return gfx::isValid(handle);
}

void Uniform::populate(const std::string& _name, gfx::UniformType::Enum _type, std::uint16_t _num /*= 1*/)
{
	dispose();

	handle = gfx::createUniform(_name.c_str(), _type, _num);
	type = _type;
	id = _name;
}

void Uniform::populate(gfx::UniformHandle _handle)
{
	handle = _handle;
	type = gfx::getUniformType(_handle);
	id = gfx::getUniformName(_handle);
}
