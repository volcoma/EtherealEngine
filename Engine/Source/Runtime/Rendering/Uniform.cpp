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

	handle = gfx::createUniform(_name.c_str(), _type, _num);
	gfx::getUniformInfo(handle, info);
}

void Uniform::populate(gfx::UniformHandle _handle)
{
	dispose();

	gfx::getUniformInfo(_handle, info);
	handle = gfx::createUniform(info.name, info.type, info.num);
}
