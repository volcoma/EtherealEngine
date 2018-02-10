#pragma once

#include "graphics.h"

namespace gfx
{

template <typename T>
class handle_impl
{
public:
	using handle_type_t = T;
	using base_type = handle_impl<T>;

	~handle_impl()
	{
		dispose();
	}

	void dispose()
	{
		if(is_valid())
		{
			bgfx::destroy(handle);
		}

		handle = invalid_handle();
	}

	bool is_valid() const
	{
		return bgfx::isValid(handle);
	}

	T native_handle() const
	{
		return handle;
	}

	static T invalid_handle()
	{
		T invalid = {bgfx::kInvalidHandle};
		return invalid;
	}

protected:
	T handle = invalid_handle();
};
}
