#pragma once
#include <memory>
#include <string>
#include <vector>

namespace audio
{
namespace priv
{
class device_impl;
}
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : device (Class)
/// <summary>
/// Wrapper over the audio device and audio context. Can enumerate and query
/// device properties.
/// </summary>
//-----------------------------------------------------------------------------
class device
{
public:
    device(int devnum = 0);
    ~device();

    //-----------------------------------------------------------------------------
    //  Name : enable ()
    /// <summary>
    /// Sets the current context.
    /// </summary>
    //-----------------------------------------------------------------------------
    void enable();

    //-----------------------------------------------------------------------------
    //  Name : disable ()
    /// <summary>
    /// Un - sets the current context.
    /// </summary>
    //-----------------------------------------------------------------------------
    void disable();

    //-----------------------------------------------------------------------------
    //  Name : is_valid ()
    /// <summary>
    /// Checks whether the device and context are valid.
    /// </summary>
    //-----------------------------------------------------------------------------
    bool is_valid() const;

    //-----------------------------------------------------------------------------
    //  Name : get_device_id ()
    /// <summary>
    /// Gets the id of the selected device.
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::string& get_device_id() const;

    //-----------------------------------------------------------------------------
    //  Name : get_vendor ()
    /// <summary>
    /// Gets vendor.
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::string& get_vendor() const;

    //-----------------------------------------------------------------------------
    //  Name : get_version ()
    /// <summary>
    /// Gets get_version.
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::string& get_version() const;

    //-----------------------------------------------------------------------------
    //  Name : get_extensions ()
    /// <summary>
    /// Gets available extensions.
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::string& get_extensions() const;

    //-----------------------------------------------------------------------------
    //  Name : enumerate_playback_devices ()
    /// <summary>
    /// Enumerate all playback devices available on the system.
    /// </summary>
    //-----------------------------------------------------------------------------
    static std::vector<std::string> enumerate_playback_devices();

    //-----------------------------------------------------------------------------
    //  Name : enumerate_capture_devices ()
    /// <summary>
    /// Enumerate all capture devices available on the system.
    /// </summary>
    //-----------------------------------------------------------------------------
    static std::vector<std::string> enumerate_capture_devices();

private:
    /// pimpl idiom
    std::unique_ptr<priv::device_impl> impl_;
};
}
