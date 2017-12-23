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

class device
{
public:
	device(int devnum = 0);
	~device();

	void enable();
	void disable();

	bool is_valid() const;

	const std::string& get_device_id() const;
	const std::string& get_info() const;
	static std::vector<std::string> enumerate();

private:
	std::unique_ptr<priv::device_impl> _impl;
};
}
