////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/joystick_impl.hpp>
#include <mml/system/err.hpp>
#include <linux/joystick.h>
#include <libudev.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <string>
#include <cstring>

namespace
{
    udev* udevContext = 0;
    udev_monitor* udevMonitor = 0;

    struct joystick_record
    {
        std::string device_node;
        std::string system_path;
        bool plugged;
    };

    typedef std::vector<joystick_record> joystick_list;
    joystick_list joystickList;

    bool is_joystick(udev_device* udevDevice)
    {
        // If anything goes wrong, we go safe and return true

        // No device to check, assume not a joystick
        if (!udevDevice)
            return false;

        const char* devnode = udev_device_get_devnode(udevDevice);

        // We only consider devices with a device node
        if (!devnode)
            return false;

        // mml doesn't support evdev yet, so make sure we only handle /js nodes
        if (!std::strstr(devnode, "/js"))
            return false;

        // Check if this device is a joystick
        if (udev_device_get_property_value(udevDevice, "ID_INPUT_JOYSTICK"))
            return true;

        // Check if this device is something that isn't a joystick
        // We do this because the absence of any ID_INPUT_ property doesn't
        // necessarily mean that the device isn't a joystick, whereas the
        // presence of any ID_INPUT_ property that isn't ID_INPUT_JOYSTICK does
        if (udev_device_get_property_value(udevDevice, "ID_INPUT_ACCELEROMETER") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_KEY") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_KEYBOARD") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_MOUSE") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_TABLET") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_TOUCHPAD") ||
            udev_device_get_property_value(udevDevice, "ID_INPUT_TOUCHSCREEN"))
            return false;

        // On some platforms (older udev), ID_INPUT_ properties are not present, instead
        // the system makes use of the ID_CLASS property to identify the device class
        const char* idClass = udev_device_get_property_value(udevDevice, "ID_CLASS");

        if (idClass)
        {
            // Check if the device class matches joystick
            if (std::strstr(idClass, "joystick"))
                return true;

            // Check if the device class matches something that isn't a joystick
            // Rationale same as above
            if (std::strstr(idClass, "accelerometer") ||
                std::strstr(idClass, "key") ||
                std::strstr(idClass, "keyboard") ||
                std::strstr(idClass, "mouse") ||
                std::strstr(idClass, "tablet") ||
                std::strstr(idClass, "touchpad") ||
                std::strstr(idClass, "touchscreen"))
                return false;
        }

        // At this point, assume it is a joystick
        return true;
    }

    void update_plugged_list(udev_device* udevDevice = NULL)
    {
        if (udevDevice)
        {
            const char* action = udev_device_get_action(udevDevice);

            if (action)
            {
                if (is_joystick(udevDevice))
                {
                    // Since is_joystick returned true, this has to succeed
                    const char* devnode = udev_device_get_devnode(udevDevice);

                    joystick_list::iterator record;

                    for (record = joystickList.begin(); record != joystickList.end(); ++record)
                    {
                        if (record->device_node == devnode)
                        {
                            if (std::strstr(action, "add"))
                            {
                                // The system path might have changed so update it
                                const char* syspath = udev_device_get_syspath(udevDevice);

                                record->plugged = true;
                                record->system_path = syspath ? syspath : "";
                                break;
                            }
                            else if (std::strstr(action, "remove"))
                            {
                                record->plugged = false;
                                break;
                            }
                        }
                    }

                    if (record == joystickList.end())
                    {
                        if (std::strstr(action, "add"))
                        {
                            // If not mapped before and it got added, map it now
                            const char* syspath = udev_device_get_syspath(udevDevice);

                            joystick_record record;
                            record.device_node = devnode;
                            record.system_path = syspath ? syspath : "";
                            record.plugged = true;

                            joystickList.push_back(record);
                        }
                        else if (std::strstr(action, "remove"))
                        {
                            // Not mapped during the initial scan, and removed (shouldn't happen)
                            mml::err() << "Trying to disconnect joystick that wasn't connected" << std::endl;
                        }
                    }
                }

                return;
            }

            // Do a full rescan if there was no action just to be sure
        }

        // Reset the plugged status of each mapping since we are doing a full rescan
        for (joystick_list::iterator record = joystickList.begin(); record != joystickList.end(); ++record)
            record->plugged = false;

        udev_enumerate* udevEnumerator = udev_enumerate_new(udevContext);

        if (!udevEnumerator)
        {
            mml::err() << "Error while creating udev enumerator" << std::endl;
            return;
        }

        int result = 0;

        result = udev_enumerate_add_match_subsystem(udevEnumerator, "input");

        if (result < 0)
        {
            mml::err() << "Error while adding udev enumerator match" << std::endl;
            return;
        }

        result = udev_enumerate_scan_devices(udevEnumerator);

        if (result < 0)
        {
            mml::err() << "Error while enumerating udev devices" << std::endl;
            return;
        }

        udev_list_entry* devices = udev_enumerate_get_list_entry(udevEnumerator);
        udev_list_entry* device;

        udev_list_entry_foreach(device, devices) {
            const char* syspath = udev_list_entry_get_name(device);
            udev_device* udevDevice = udev_device_new_from_syspath(udevContext, syspath);

            if (udevDevice && is_joystick(udevDevice))
            {
                // Since is_joystick returned true, this has to succeed
                const char* devnode = udev_device_get_devnode(udevDevice);

                joystick_list::iterator record;

                // Check if the device node has been mapped before
                for (record = joystickList.begin(); record != joystickList.end(); ++record)
                {
                    if (record->device_node == devnode)
                    {
                        record->plugged = true;
                        break;
                    }
                }

                // If not mapped before, map it now
                if (record == joystickList.end())
                {
                    joystick_record record;
                    record.device_node = devnode;
                    record.system_path = syspath;
                    record.plugged = true;

                    joystickList.push_back(record);
                }
            }

            udev_device_unref(udevDevice);
        }

        udev_enumerate_unref(udevEnumerator);
    }

    bool has_monitor_event()
    {
        // This will not fail since we make sure udevMonitor is valid
        int monitorFd = udev_monitor_get_fd(udevMonitor);

        fd_set descriptorSet;
        FD_ZERO(&descriptorSet);
        FD_SET(monitorFd, &descriptorSet);
        timeval timeout = {0, 0};

        return (select(monitorFd + 1, &descriptorSet, NULL, NULL, &timeout) > 0) &&
               FD_ISSET(monitorFd, &descriptorSet);
    }

    // Get a property value from a udev device
    const char* get_udev_attribute(udev_device* udevDevice, const std::string& attributeName)
    {
        return udev_device_get_property_value(udevDevice, attributeName.c_str());
    }

    // Get a system attribute from a USB device
    const char* get_usb_attribute(udev_device* udevDevice, const std::string& attributeName)
    {
        udev_device* udevDeviceParent = udev_device_get_parent_with_subsystem_devtype(udevDevice, "usb", "usb_device");

        if (!udevDeviceParent)
            return NULL;

        return udev_device_get_sysattr_value(udevDeviceParent, attributeName.c_str());
    }

    // Get a USB attribute for a joystick as an unsigned int
    unsigned int get_usb_attribute_uint(udev_device* udevDevice, const std::string& attributeName)
    {
        if (!udevDevice)
            return 0;

        const char* attribute = get_usb_attribute(udevDevice, attributeName);
        unsigned int value = 0;

        if (attribute)
            value = static_cast<unsigned int>(std::strtoul(attribute, NULL, 16));

        return value;
    }

    // Get a udev property value for a joystick as an unsigned int
    unsigned int get_udev_attribute_uint(udev_device* udevDevice, const std::string& attributeName)
    {
        if (!udevDevice)
            return 0;

        const char* attribute = get_udev_attribute(udevDevice, attributeName);
        unsigned int value = 0;

        if (attribute)
            value = static_cast<unsigned int>(std::strtoul(attribute, NULL, 16));

        return value;
    }

    // Get the joystick vendor id
    unsigned int get_joystick_vendor_id(unsigned int index)
    {
        if (!udevContext)
        {
            mml::err() << "Failed to get vendor ID of joystick " << joystickList[index].device_node << std::endl;
            return 0;
        }

        udev_device* udevDevice = udev_device_new_from_syspath(udevContext, joystickList[index].system_path.c_str());

        if (!udevDevice)
        {
            mml::err() << "Failed to get vendor ID of joystick " << joystickList[index].device_node << std::endl;
            return 0;
        }

        unsigned int id = 0;

        // First try using udev
        id = get_udev_attribute_uint(udevDevice, "ID_VENDOR_ID");

        if (id)
        {
            udev_device_unref(udevDevice);
            return id;
        }

        // Fall back to using USB attribute
        id = get_usb_attribute_uint(udevDevice, "idVendor");

        udev_device_unref(udevDevice);

        if (id)
            return id;

        mml::err() << "Failed to get vendor ID of joystick " << joystickList[index].device_node << std::endl;

        return 0;
    }

    // Get the joystick product id
    unsigned int get_joystick_product_id(unsigned int index)
    {
        if (!udevContext)
        {
            mml::err() << "Failed to get product ID of joystick " << joystickList[index].device_node << std::endl;
            return 0;
        }

        udev_device* udevDevice = udev_device_new_from_syspath(udevContext, joystickList[index].system_path.c_str());

        if (!udevDevice)
        {
            mml::err() << "Failed to get product ID of joystick " << joystickList[index].device_node << std::endl;
            return 0;
        }

        unsigned int id = 0;

        // First try using udev
        id = get_udev_attribute_uint(udevDevice, "ID_MODEL_ID");

        if (id)
        {
            udev_device_unref(udevDevice);
            return id;
        }

        // Fall back to using USB attribute
        id = get_usb_attribute_uint(udevDevice, "idProduct");

        udev_device_unref(udevDevice);

        if (id)
            return id;

        mml::err() << "Failed to get product ID of joystick " << joystickList[index].device_node << std::endl;

        return 0;
    }

    // Get the joystick name
    std::string get_joystick_name(unsigned int index)
    {
        std::string devnode = joystickList[index].device_node;

        // First try using ioctl with JSIOCGNAME
        int fd = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);

        if (fd >= 0)
        {
            // Get the name
            char name[128];
            std::memset(name, 0, sizeof(name));

            int result = ioctl(fd, JSIOCGNAME(sizeof(name)), name);

            ::close(fd);

            if (result >= 0)
                return std::string(name);
        }

        // Fall back to manual USB chain walk via udev
        if (udevContext)
        {
            udev_device* udevDevice = udev_device_new_from_syspath(udevContext, joystickList[index].system_path.c_str());

            if (udevDevice)
            {
                const char* product = get_usb_attribute(udevDevice, "product");
                udev_device_unref(udevDevice);

                if (product)
                    return std::string(product);
            }
        }

        mml::err() << "Unable to get name for joystick " << devnode << std::endl;

        return std::string("Unknown joystick");
    }
}


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
joystick_impl::joystick_impl() :
_file(-1)
{
    std::fill(_mapping, _mapping + ABS_MAX + 1, 0);
}


////////////////////////////////////////////////////////////
void joystick_impl::initialize()
{
    udevContext = udev_new();

    if (!udevContext)
    {
        mml::err() << "Failed to create udev context, joystick support not available" << std::endl;
        return;
    }

    udevMonitor = udev_monitor_new_from_netlink(udevContext, "udev");

    if (!udevMonitor)
    {
        err() << "Failed to create udev monitor, joystick connections and disconnections won't be notified" << std::endl;
    }
    else
    {
        int error = udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "input", NULL);

        if (error < 0)
        {
            err() << "Failed to add udev monitor filter, joystick connections and disconnections won't be notified: " << error << std::endl;

            udev_monitor_unref(udevMonitor);
            udevMonitor = 0;
        }
        else
        {
            error = udev_monitor_enable_receiving(udevMonitor);

            if (error < 0)
            {
                err() << "Failed to enable udev monitor, joystick connections and disconnections won't be notified: " << error << std::endl;

                udev_monitor_unref(udevMonitor);
                udevMonitor = 0;
            }
        }
    }

    // Do an initial scan
    update_plugged_list();
}


////////////////////////////////////////////////////////////
void joystick_impl::cleanup()
{
    // Unreference the udev monitor to destroy it
    if (udevMonitor)
    {
        udev_monitor_unref(udevMonitor);
        udevMonitor = 0;
    }

    // Unreference the udev context to destroy it
    if (udevContext)
    {
        udev_unref(udevContext);
        udevContext = 0;
    }
}


////////////////////////////////////////////////////////////
bool joystick_impl::is_connected(unsigned int index)
{
    // See if we can skip scanning if udev monitor is available
    if (!udevMonitor)
    {
        // udev monitor is not available, perform a scan every query
        update_plugged_list();
    }
    else if (has_monitor_event())
    {
        // Check if new joysticks were added/removed since last update
        udev_device* udevDevice = udev_monitor_receive_device(udevMonitor);

        // If we can get the specific device, we check that,
        // otherwise just do a full scan if udevDevice == NULL
        update_plugged_list(udevDevice);

        if (udevDevice)
            udev_device_unref(udevDevice);
    }

    if (index >= joystickList.size())
        return false;

    // Then check if the joystick is connected
    return joystickList[index].plugged;
}

////////////////////////////////////////////////////////////
bool joystick_impl::open(unsigned int index)
{
    if (index >= joystickList.size())
        return false;

    if (joystickList[index].plugged)
    {
        std::string devnode = joystickList[index].device_node;

        // Open the joystick's file descriptor (read-only and non-blocking)
        _file = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);
        if (_file >= 0)
        {
            // Retrieve the axes mapping
            ioctl(_file, JSIOCGAXMAP, _mapping);

            // Get info
            _identification.name = get_joystick_name(index);

            if (udevContext)
            {
                _identification.vendor_id  = get_joystick_vendor_id(index);
                _identification.product_id = get_joystick_product_id(index);
            }

            // Reset the joystick state
            _state = joystick_state();

            return true;
        }
        else
        {
            err() << "Failed to open joystick " << devnode << ": " << errno << std::endl;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////
void joystick_impl::close()
{
    ::close(_file);
    _file = -1;
}


////////////////////////////////////////////////////////////
joystick_caps joystick_impl::get_capabilities() const
{
    joystick_caps caps;

    if (_file < 0)
        return caps;

    // Get the number of buttons
    char button_count;
    ioctl(_file, JSIOCGBUTTONS, &button_count);
    caps.button_count = button_count;
    if (caps.button_count > joystick::button_count)
        caps.button_count = joystick::button_count;

    // Get the supported axes
    char axesCount;
    ioctl(_file, JSIOCGAXES, &axesCount);
    for (int i = 0; i < axesCount; ++i)
    {
        switch (_mapping[i])
        {
            case ABS_X:        caps.axes[joystick::X]    = true; break;
            case ABS_Y:        caps.axes[joystick::Y]    = true; break;
            case ABS_Z:
            case ABS_THROTTLE: caps.axes[joystick::Z]    = true; break;
            case ABS_RZ:
            case ABS_RUDDER:   caps.axes[joystick::R]    = true; break;
            case ABS_RX:       caps.axes[joystick::U]    = true; break;
            case ABS_RY:       caps.axes[joystick::V]    = true; break;
            case ABS_HAT0X:    caps.axes[joystick::PovX] = true; break;
            case ABS_HAT0Y:    caps.axes[joystick::PovY] = true; break;
            default:           break;
        }
    }

    return caps;
}


////////////////////////////////////////////////////////////
joystick::identification joystick_impl::get_identification() const
{
    return _identification;
}


////////////////////////////////////////////////////////////
joystick_state joystick_impl::joystick_impl::update()
{
    if (_file < 0)
    {
        _state = joystick_state();
        return _state;
    }

    // pop events from the joystick file
    js_event joyState;
    int result = read(_file, &joyState, sizeof(joyState));
    while (result > 0)
    {
        switch (joyState.type & ~JS_EVENT_INIT)
        {
            // An axis was moved
            case JS_EVENT_AXIS:
            {
                float value = joyState.value * 100.f / 32767.f;

                if (joyState.number < ABS_MAX + 1)
                {
                    switch (_mapping[joyState.number])
                    {
                        case ABS_X:        _state.axes[joystick::X]    = value; break;
                        case ABS_Y:        _state.axes[joystick::Y]    = value; break;
                        case ABS_Z:
                        case ABS_THROTTLE: _state.axes[joystick::Z]    = value; break;
                        case ABS_RZ:
                        case ABS_RUDDER:   _state.axes[joystick::R]    = value; break;
                        case ABS_RX:       _state.axes[joystick::U]    = value; break;
                        case ABS_RY:       _state.axes[joystick::V]    = value; break;
                        case ABS_HAT0X:    _state.axes[joystick::PovX] = value; break;
                        case ABS_HAT0Y:    _state.axes[joystick::PovY] = value; break;
                        default:           break;
                    }
                }
                break;
            }

            // A button was pressed
            case JS_EVENT_BUTTON:
            {
                if (joyState.number < joystick::button_count)
                    _state.buttons[joyState.number] = (joyState.value != 0);
                break;
            }
        }

        result = read(_file, &joyState, sizeof(joyState));
    }

    // Check the connection state of the joystick
    // read() returns -1 and errno != EGAIN if it's no longer connected
    // We need to check the result of read() as well, since errno could
    // have been previously set by some other function call that failed
    // result can be either negative or 0 at this point
    // If result is 0, assume the joystick is still connected
    // If result is negative, check errno and disconnect if it is not EAGAIN
    _state.connected = (!result || (errno == EAGAIN));

    return _state;
}

} // namespace priv

} // namespace mml
