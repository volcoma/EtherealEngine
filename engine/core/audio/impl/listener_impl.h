#pragma once
#include "../types.h"

namespace audio
{
namespace priv
{
class listener_impl
{
public:
    void set_volume(float volume);
    void set_position(const float3& position);
    void set_velocity(const float3& velocity);
    void set_orientation(const float3& direction, const float3& up);
};
}
}
