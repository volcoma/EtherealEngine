#pragma once

#include <core/common/basetypes.hpp>

namespace runtime
{
class camera_system
{
public:
    camera_system();
    ~camera_system();
    //-----------------------------------------------------------------------------
    //  Name : frame_update (virtual )
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void frame_update(delta_t dt);
};
} // namespace runtime
