#pragma once
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
	ALCdevice* _device = nullptr;
	ALCcontext* _context = nullptr;

	std::string _device_id;

	std::string _version;
	std::string _vendor;
	std::string _extensions;
};
}
}
