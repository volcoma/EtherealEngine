#include "Uniform.h"

Uniform::~Uniform()
{
	dispose();
}

void Uniform::dispose()
{
	if (isValid())
		gfx::destroyUniform(handle);

	handle = { bgfx::invalidHandle };
}

bool Uniform::isValid() const
{
	return gfx::isValid(handle);
}

void Uniform::populate(const std::string& _name, gfx::UniformType::Enum _type, std::uint16_t _num /*= 1*/)
{
	dispose();

	type = _type;
	id = _name;
	num = _num;
	handle = gfx::createUniform(_name.c_str(), _type, _num);
}

void Uniform::populate(gfx::UniformHandle _handle)
{
	dispose();

	type = gfx::getUniformType(_handle);
	id = gfx::getUniformName(_handle);
	num = gfx::getUniformNum(_handle);
	handle = gfx::createUniform(id.c_str(), type, num);
}
