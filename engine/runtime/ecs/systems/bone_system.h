#pragma once

#include <core/common/basetypes.hpp>

namespace runtime
{
class bone_system
{
public:
    bone_system();
    ~bone_system();
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
