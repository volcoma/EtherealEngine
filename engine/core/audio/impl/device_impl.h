#pragma once
#include <string>
#include <vector>

#include <AL/alc.h>
#include <map>

namespace audio
{

class device_impl
{
public:
	device_impl(int devnum = 0);
	~device_impl();

	void enable();
	void disable();

	bool is_valid() const;

	const std::string& get_device_id() const;
	const std::string& get_info() const;
	static std::vector<std::string> enumerate();

private:
	bool init(int devnum);
	void quit();

	ALCdevice* dev = nullptr;
	ALCcontext* ctx = nullptr;

	std::string _device_id;
	std::string _info;
};
}
