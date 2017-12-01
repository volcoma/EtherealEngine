#include "uniform.h"

namespace gfx
{

uniform::uniform(const std::string& _name, uniform_type _type, std::uint16_t _num /*= 1*/)
{
	handle = gfx::create_uniform(_name.c_str(), _type, _num);
	gfx::get_uniform_info(handle, info);
}

uniform::uniform(handle_type_t _handle)
{
	gfx::get_uniform_info(_handle, info);
	handle = gfx::create_uniform(info.name, info.type, info.num);
}
}
