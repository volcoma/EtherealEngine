////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/sensor_manager.hpp>
#include <mml/system/err.hpp>

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
sensor_manager& sensor_manager::get_instance()
{
    static sensor_manager instance;
    return instance;
}


////////////////////////////////////////////////////////////
bool sensor_manager::is_available(sensor::type sensor)
{
    return m_sensors[sensor].available;
}


////////////////////////////////////////////////////////////
void sensor_manager::set_enabled(sensor::type sensor, bool enabled)
{
    if (m_sensors[sensor].available)
    {
        m_sensors[sensor].enabled = enabled;
        m_sensors[sensor].sensor.set_enabled(enabled);
    }
    else
    {
        err() << "Warning: trying to enable a sensor that is not available (call sensor::is_available to check it)" << std::endl;
    }
}


////////////////////////////////////////////////////////////
bool sensor_manager::is_enabled(sensor::type sensor) const
{
    return m_sensors[sensor].enabled;
}


////////////////////////////////////////////////////////////
std::array<float, 3> sensor_manager::get_value(sensor::type sensor) const
{
    return m_sensors[sensor].value;
}


////////////////////////////////////////////////////////////
void sensor_manager::update()
{
    for (int i = 0; i < sensor::count; ++i)
    {
        // Only process available sensors
        if (m_sensors[i].available)
            m_sensors[i].value = m_sensors[i].sensor.update();
    }
}


////////////////////////////////////////////////////////////
sensor_manager::sensor_manager()
{
    // Global sensor initialization
    sensor_impl::initialize();

    // Per sensor initialization
    for (int i = 0; i < sensor::count; ++i)
    {
        // Check which sensors are available
        m_sensors[i].available = sensor_impl::is_available(static_cast<sensor::type>(i));

        // Open the available sensors
        if (m_sensors[i].available)
        {
            m_sensors[i].sensor.open(static_cast<sensor::type>(i));
            m_sensors[i].sensor.set_enabled(false);
        }
    }
}

////////////////////////////////////////////////////////////
sensor_manager::~sensor_manager()
{
    // Per sensor cleanup
    for (int i = 0; i < sensor::count; ++i)
    {
        if (m_sensors[i].available)
            m_sensors[i].sensor.close();
    }

    // Global sensor cleanup
    sensor_impl::cleanup();
}

} // namespace priv

} // namespace mml
