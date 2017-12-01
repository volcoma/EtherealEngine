#pragma once

#include "handle_impl.h"

namespace gfx
{
struct index_buffer : public handle_impl<index_buffer_handle>
{
    index_buffer() = default;
	//-----------------------------------------------------------------------------
	//  Name : index_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	index_buffer(const memory_view* _mem, std::uint16_t _flags = BGFX_BUFFER_NONE);
};
}
