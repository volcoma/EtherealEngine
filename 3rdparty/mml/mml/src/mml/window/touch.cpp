////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/touch.hpp>
#include <mml/window/input_impl.hpp>


namespace mml
{
////////////////////////////////////////////////////////////
bool touch::is_down(unsigned int finger)
{
    return priv::input_impl::is_touch_down(finger);
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> touch::get_position(unsigned int finger)
{
    return priv::input_impl::get_touch_position(finger);
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> touch::get_position(unsigned int finger, const window& relativeTo)
{
    return priv::input_impl::get_touch_position(finger, relativeTo);
}

} // namespace mml
