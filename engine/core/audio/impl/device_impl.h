#pragma once
#include <memory>
#include <string>
#include <vector>

#include <AL/alc.h>
#include <map>

namespace audio
{
namespace priv
{
class device_impl
{
public:
    device_impl(int devnum);
    ~device_impl();

    void enable();
    void disable();

    bool is_valid() const;

    const std::string& get_device_id() const;
    const std::string& get_version() const;
    const std::string& get_vendor() const;
    const std::string& get_extensions() const;

    static std::vector<std::string> enumerate_playback_devices();
    static std::vector<std::string> enumerate_capture_devices();

private:
    struct deleter
    {
        void operator()(ALCdevice *obj);
        void operator()(ALCcontext *obj);
    };

    std::unique_ptr<ALCdevice, deleter> device_;
    std::unique_ptr<ALCcontext, deleter> context_;

    std::string device_id_;

    std::string version_;
    std::string vendor_;
    std::string extensions_;
};
}
}
