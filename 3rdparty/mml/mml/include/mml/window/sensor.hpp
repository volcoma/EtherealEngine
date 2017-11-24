#ifndef MML_SENSOR_HPP
#define MML_SENSOR_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <array>

namespace mml
{
////////////////////////////////////////////////////////////
/// \brief Give access to the real-time state of the sensors
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API sensor
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief sensor type
    ///
    ////////////////////////////////////////////////////////////
    enum type
    {
        accelerometer,    ///< Measures the raw acceleration (m/s^2)
        gyroscope,        ///< Measures the raw rotation rates (degrees/s)
        magnetometer,     ///< Measures the ambient magnetic field (micro-teslas)
        gravity,          ///< Measures the direction and intensity of gravity, independent of device acceleration (m/s^2)
        user_acceleration, ///< Measures the direction and intensity of device acceleration, independent of the gravity (m/s^2)
        orientation,      ///< Measures the absolute 3D orientation (degrees)

        count             ///< Keep last -- the total number of sensor types
    };

    ////////////////////////////////////////////////////////////
    /// \brief Check if a sensor is available on the underlying platform
    ///
    /// \param sensor sensor to check
    ///
    /// \return True if the sensor is available, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_available(type sensor);

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable a sensor
    ///
    /// All sensors are disabled by default, to avoid consuming too
    /// much battery power. Once a sensor is enabled, it starts
    /// sending events of the corresponding type.
    ///
    /// This function does nothing if the sensor is unavailable.
    ///
    /// \param sensor  sensor to enable
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    static void set_enabled(type sensor, bool enabled);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current sensor value
    ///
    /// \param sensor sensor to read
    ///
    /// \return The current sensor value
    ///
    ////////////////////////////////////////////////////////////
    static std::array<float, 3> get_value(type sensor);
};

} // namespace mml


#endif // MML_SENSOR_HPP


////////////////////////////////////////////////////////////
/// \class mml::sensor
/// \ingroup window
///
/// mml::sensor provides an interface to the state of the
/// various sensors that a device provides. It only contains static
/// functions, so it's not meant to be instantiated.
///
/// This class allows users to query the sensors values at any
/// time and directly, without having to deal with a window and
/// its events. Compared to the sensor_changed event, mml::sensor
/// can retrieve the state of a sensor at any time (you don't need to
/// store and update its current value on your side).
///
/// Depending on the OS and hardware of the device (phone, tablet, ...),
/// some sensor types may not be available. You should always check
/// the availability of a sensor before trying to read it, with the
/// mml::sensor::is_available function.
///
/// You may wonder why some sensor types look so similar, for example
/// Accelerometer and Gravity / user_acceleration. The first one
/// is the raw measurement of the acceleration, and takes into account
/// both the earth gravity and the user movement. The others are
/// more precise: they provide these components separately, which is
/// usually more useful. In fact they are not direct sensors, they
/// are computed internally based on the raw acceleration and other sensors.
/// This is exactly the same for Gyroscope vs orientation.
///
/// Because sensors consume a non-negligible amount of current, they are
/// all disabled by default. You must call mml::sensor::set_enabled for each
/// sensor in which you are interested.
///
/// Usage example:
/// \code
/// if (mml::sensor::is_available(mml::sensor::Gravity))
/// {
///     // gravity sensor is available
/// }
///
/// // enable the gravity sensor
/// mml::sensor::set_enabled(mml::sensor::Gravity, true);
///
/// // get the current value of gravity
/// mml::std::array<float, 3> gravity = mml::sensor::get_value(mml::sensor::Gravity);
/// \endcode
///
////////////////////////////////////////////////////////////
