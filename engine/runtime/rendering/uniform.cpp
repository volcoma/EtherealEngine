#include "uniform.h"

uniform::~uniform()
{
	dispose();
}

void uniform::dispose()
{
	if (is_valid())
		gfx::destroyUniform(handle);

	handle = { bgfx::kInvalidHandle };
}

bool uniform::is_valid() const
{
	return gfx::isValid(handle);
}

void uniform::populate(const std::string& _name, gfx::UniformType::Enum _type, std::uint16_t _num /*= 1*/)
{
	dispose();

	handle = gfx::createUniform(_name.c_str(), _type, _num);
	gfx::getUniformInfo(handle, info);
}

void uniform::populate(gfx::UniformHandle _handle)
{
	dispose();

	gfx::getUniformInfo(_handle, info);
	handle = gfx::createUniform(info.name, info.type, info.num);
}
