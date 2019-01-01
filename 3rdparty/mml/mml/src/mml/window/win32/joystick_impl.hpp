#ifndef MML_joystick_implWIN32_HPP
#define MML_joystick_implWIN32_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#ifdef _WIN32_WINDOWS
    #undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS      0x0501
#define _WIN32_WINNT        0x0501
#define DIRECTINPUT_VERSION 0x0800
#include <mml/window/joystick.hpp>
#include <mml/window/joystick_impl.hpp>
#include <windows.h>
#include <mmsystem.h>
#include <dinput.h>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Windows implementation of joysticks
///
////////////////////////////////////////////////////////////
class joystick_impl
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global initialization of the joystick module
    ///
    ////////////////////////////////////////////////////////////
    static void initialize();

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global cleanup of the joystick module
    ///
    ////////////////////////////////////////////////////////////
    static void cleanup();

    ////////////////////////////////////////////////////////////
    /// \brief Check if a joystick is currently connected
    ///
    /// \param index Index of the joystick to check
    ///
    /// \return True if the joystick is connected, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_connected(unsigned int index);

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable lazy enumeration updates
    ///
    /// \param status Whether to rely on windows triggering enumeration updates
    ///
    ////////////////////////////////////////////////////////////
    static void set_lazy_updates(bool status);

    ////////////////////////////////////////////////////////////
    /// \brief Update the connection status of all joysticks
    ///
    ////////////////////////////////////////////////////////////
    static void update_connections();

    ////////////////////////////////////////////////////////////
    /// \brief Open the joystick
    ///
    /// \param index Index assigned to the joystick
    ///
    /// \return True on success, false on failure
    ///
    ////////////////////////////////////////////////////////////
    bool open(unsigned int index);

    ////////////////////////////////////////////////////////////
    /// \brief Close the joystick
    ///
    ////////////////////////////////////////////////////////////
    void close();

    ////////////////////////////////////////////////////////////
    /// \brief Get the joystick capabilities
    ///
    /// \return joystick capabilities
    ///
    ////////////////////////////////////////////////////////////
    joystick_caps get_capabilities() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the joystick identification
    ///
    /// \return joystick identification
    ///
    ////////////////////////////////////////////////////////////
    joystick::identification get_identification() const;

    ////////////////////////////////////////////////////////////
    /// \brief Update the joystick and get its new state
    ///
    /// \return joystick state
    ///
    ////////////////////////////////////////////////////////////
    joystick_state update();

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global initialization of the joystick module (DInput)
    ///
    ////////////////////////////////////////////////////////////
    static void initialize_dinput();

    ////////////////////////////////////////////////////////////
    /// \brief Perform the global cleanup of the joystick module (DInput)
    ///
    ////////////////////////////////////////////////////////////
    static void cleanup_dinput();

    ////////////////////////////////////////////////////////////
    /// \brief Check if a joystick is currently connected (DInput)
    ///
    /// \param index Index of the joystick to check
    ///
    /// \return True if the joystick is connected, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_connected_dinput(unsigned int index);

    ////////////////////////////////////////////////////////////
    /// \brief Update the connection status of all joysticks (DInput)
    ///
    ////////////////////////////////////////////////////////////
    static void update_connections_dinput();

    ////////////////////////////////////////////////////////////
    /// \brief Open the joystick (DInput)
    ///
    /// \param index Index assigned to the joystick
    ///
    /// \return True on success, false on failure
    ///
    ////////////////////////////////////////////////////////////
    bool open_dinput(unsigned int index);

    ////////////////////////////////////////////////////////////
    /// \brief Close the joystick (DInput)
    ///
    ////////////////////////////////////////////////////////////
    void close_dinput();

    ////////////////////////////////////////////////////////////
    /// \brief Get the joystick capabilities (DInput)
    ///
    /// \return joystick capabilities
    ///
    ////////////////////////////////////////////////////////////
    joystick_caps get_capabilities_dinput() const;

    ////////////////////////////////////////////////////////////
    /// \brief Update the joystick and get its new state (DInput)
    ///
    /// \return joystick state
    ///
    ////////////////////////////////////////////////////////////
    joystick_state update_dinput();

private:

    ////////////////////////////////////////////////////////////
    /// \brief Device enumeration callback function passed to EnumDevices in updateConnections
    ///
    /// \param deviceInstance Device object instance
    /// \param userData       User data (unused)
    ///
    /// \return DIENUM_CONTINUE to continue enumerating devices or DIENUM_STOP to stop
    ///
    ////////////////////////////////////////////////////////////
    static BOOL CALLBACK device_enumeration_callback(const DIDEVICEINSTANCE* deviceInstance, void* userData);

    ////////////////////////////////////////////////////////////
    /// \brief Device object enumeration callback function passed to EnumObjects in open
    ///
    /// \param deviceObjectInstance Device object instance
    /// \param userData             User data (pointer to our joystick_impl object)
    ///
    /// \return DIENUM_CONTINUE to continue enumerating objects or DIENUM_STOP to stop
    ///
    ////////////////////////////////////////////////////////////
    static BOOL CALLBACK device_object_enumeration_callback(const DIDEVICEOBJECTINSTANCE* deviceObjectInstance, void* userData);

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    unsigned int             m_index;                          ///< Index of the joystick
    JOYCAPS                  m_caps;                           ///< joystick capabilities
    IDirectInputDevice8W*    m_device;                         ///< DirectInput 8.x device
    DIDEVCAPS                m_deviceCaps;                     ///< DirectInput device capabilities
    int                      m_axes[joystick::axis_count];      ///< Offsets to the bytes containing the axes states, -1 if not available
    int                      m_buttons[joystick::button_count]; ///< Offsets to the bytes containing the button states, -1 if not available
    joystick::identification m_identification;                 ///< joystick identification
};

} // namespace priv

} // namespace sf


#endif // MML_joystick_implWIN32_HPP
