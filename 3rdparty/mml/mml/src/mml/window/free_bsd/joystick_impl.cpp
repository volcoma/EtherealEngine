////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/joystick_impl.hpp>
#include <mml/system/err.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <string>
#include <utility>

////////////////////////////////////////////////////////////
/// \brief This file implements FreeBSD driver joystick
///
/// It has been tested on a Saitek gamepad with 12 buttons,
/// 2 analog axis and one hat.
///
/// Note: old joy(4) drivers are not supported and no one use that
/// anymore.
////////////////////////////////////////////////////////////


namespace
{
    std::map<unsigned int, std::string> plugged;
    std::map<int, std::pair<int, int> > hatValueMap;

    bool is_joystick(const char *name)
    {
        int file = ::open(name, O_RDONLY | O_NONBLOCK);

        if (file < 0)
            return false;

        report_desc_t desc = hid_get_report_desc(file);

        if (!desc)
        {
            ::close(file);
            return false;
        }

        int id = hid_get_report_id(file);
        hid_data_t data = hid_start_parse(desc, 1 << hid_input, id);

        if (!data)
        {
            hid_dispose_report_desc(desc);
            ::close(file);
            return false;
        }

        hid_item_t item;

        // Assume it isn't
        bool result = false;

        while (hid_get_item(data, &item) > 0)
        {
            if ((item.kind == hid_collection) && (HID_PAGE(item.usage) == HUP_GENERIC_DESKTOP))
            {
                if ((HID_USAGE(item.usage) == HUG_JOYSTICK) || (HID_USAGE(item.usage) == HUG_GAME_PAD))
                {
                    result = true;
                }
            }
        }

        hid_end_parse(data);
        hid_dispose_report_desc(desc);
        ::close(file);

        return result;
    }

    void update_plugged_list()
    {
        /*
         * Devices /dev/uhid<x> are shared between joystick and any other
         * human interface device. We need to iterate over all found devices
         * and check if they are joysticks. The index of JoystickImpl::open
         * does not match the /dev/uhid<index> device!
         */
        DIR* directory = opendir("/dev");

        if (directory)
        {
            int joystickCount = 0;
            struct dirent* directoryEntry = readdir(directory);

            while (directoryEntry && joystickCount < mml::Joystick::Count)
            {
                if (!std::strncmp(directoryEntry->d_name, "uhid", 4))
                {
                    std::string name("/dev/");
                    name += directoryEntry->d_name;

                    if (is_joystick(name.c_str()))
                        plugged[joystickCount++] = name;
                }

                directoryEntry = readdir(directory);
            }

            closedir(directory);
        }
    }

    int usageToAxis(int usage)
    {
        switch (usage)
        {
            case HUG_X:  return mml::Joystick::X;
            case HUG_Y:  return mml::Joystick::Y;
            case HUG_Z:  return mml::Joystick::Z;
            case HUG_RZ: return mml::Joystick::R;
            case HUG_RX: return mml::Joystick::U;
            case HUG_RY: return mml::Joystick::V;
            default:     return -1;
        }
    }

    void hatValueToSfml(int value, mml::priv::JoystickState& state)
    {
        state.axes[mml::Joystick::PovX] = hatValueMap[value].first;
        state.axes[mml::Joystick::PovY] = hatValueMap[value].second;
    }
}


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
void joystick_impl::initialize()
{
    hid_init(NULL);

    // Do an initial scan
    update_plugged_list();

    // Map of hat values
    hatValueMap[0] = std::make_pair(   0,    0); // center

    hatValueMap[1] = std::make_pair(   0, -100); // top
    hatValueMap[3] = std::make_pair( 100,    0); // right
    hatValueMap[5] = std::make_pair(   0,  100); // bottom
    hatValueMap[7] = std::make_pair(-100,    0); // left

    hatValueMap[2] = std::make_pair( 100, -100); // top-right
    hatValueMap[4] = std::make_pair( 100,  100); // bottom-right
    hatValueMap[6] = std::make_pair(-100,  100); // bottom-left
    hatValueMap[8] = std::make_pair(-100, -100); // top-left
}


////////////////////////////////////////////////////////////
void joystick_impl::cleanup()
{
}


////////////////////////////////////////////////////////////
bool joystick_impl::isConnected(unsigned int index)
{
    return plugged.find(index) != plugged.end();
}


////////////////////////////////////////////////////////////
bool joystick_impl::open(unsigned int index)
{
    if (isConnected(index))
    {
        // Open the joystick's file descriptor (read-only and non-blocking)
        _file = ::open(plugged[index].c_str(), O_RDONLY | O_NONBLOCK);
        if (_file >= 0)
        {
            // Reset the joystick state
            _state = JoystickState();

            // Get the report descriptor
            m_desc = hid_get_report_desc(_file);
            if (!m_desc)
            {
                ::close(_file);
                return false;
            }

            // And the id
            m_id = hid_get_report_id(_file);

            // Then allocate a buffer for data retrieval
            m_length = hid_report_size(m_desc, hid_input, m_id);
            m_buffer.resize(m_length);

            _state.connected = true;

            return true;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////
void joystick_impl::close()
{
    ::close(_file);
    hid_dispose_report_desc(m_desc);
}


////////////////////////////////////////////////////////////
JoystickCaps joystick_impl::get_capabilities() const
{
    JoystickCaps caps;
    hid_item_t item;

    hid_data_t data = hid_start_parse(m_desc, 1 << hid_input, m_id);

    while (hid_get_item(data, &item))
    {
        if (item.kind == hid_input)
        {
            int usage = HID_USAGE(item.usage);

            if (usage == HUP_BUTTON)
            {
                caps.buttonCount++;
                break;
            }
            else if (usage == HUP_GENERIC_DESKTOP)
            {
                int axis = usageToAxis(usage);

                if (usage == HUG_HAT_SWITCH)
                {
                    caps.axes[Joystick::PovX] = true;
                    caps.axes[Joystick::PovY] = true;
                }
                else if (axis != -1)
                {
                    caps.axes[axis] = true;
                }
            }
        }
    }

    hid_end_parse(data);

    return caps;
}


////////////////////////////////////////////////////////////
Joystick::Identification joystick_impl::getIdentification() const
{
    return _identification;
}


////////////////////////////////////////////////////////////
JoystickState joystick_impl::joystick_impl::update()
{
    while (read(_file, &m_buffer[0], m_length) == m_length)
    {
        hid_data_t data = hid_start_parse(m_desc, 1 << hid_input, m_id);

        // No memory?
        if (!data)
            continue;

        int buttonIndex = 0;
        hid_item_t item;

        while (hid_get_item(data, &item))
        {
            if (item.kind == hid_input)
            {
                int usage = HID_USAGE(item.usage);

                if (usage == HUP_BUTTON)
                {
                    _state.buttons[buttonIndex++] = hid_get_data(&m_buffer[0], &item);
                }
                else if (usage == HUP_GENERIC_DESKTOP)
                {
                    int value = hid_get_data(&m_buffer[0], &item);
                    int axis = usageToAxis(usage);

                    if (usage == HUG_HAT_SWITCH)
                    {
                        hatValueToSfml(value, _state);
                    }
                    else if (axis != -1)
                    {
                        int minimum = item.logical_minimum;
                        int maximum = item.logical_maximum;

                        value = (value - minimum) * 200 / (maximum - minimum) - 100;
                        _state.axes[axis] = value;
                    }
                }
            }
        }

        hid_end_parse(data);
    }

    return _state;
}

} // namespace priv

} // namespace sf
