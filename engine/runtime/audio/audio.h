#pragma once

#include "core/audio/library.h"
#include <memory>
#include <vector>

namespace runtime
{
class audio_device
{
public:
	audio_device();

private:
	std::unique_ptr<audio::device> _device;
};
}
