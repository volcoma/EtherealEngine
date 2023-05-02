#pragma once

#include "handle_impl.h"
#include <string>

namespace gfx
{
struct uniform : public handle_impl<uniform_handle>
{
    uniform() = default;
    //-----------------------------------------------------------------------------
    //  Name : populate ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    uniform(const std::string& _name, uniform_type _type, std::uint16_t _num = 1);

    //-----------------------------------------------------------------------------
    //  Name : populate ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    uniform(handle_type_t _handle);

    /// Uniform info
    uniform_info info;
};
} // namespace gfx
