////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/sensor.hpp>
#include <mml/window/sensor_manager.hpp>


namespace mml
{

////////////////////////////////////////////////////////////
bool sensor::is_available(type sensor)
{
    return priv::sensor_manager::get_instance().is_available(sensor);
}

////////////////////////////////////////////////////////////
void sensor::set_enabled(type sensor, bool enabled)
{
    return priv::sensor_manager::get_instance().set_enabled(sensor, enabled);
}

////////////////////////////////////////////////////////////
std::array<float, 3> sensor::get_value(type sensor)
{
    return priv::sensor_manager::get_instance().get_value(sensor);
}

} // namespace mml
