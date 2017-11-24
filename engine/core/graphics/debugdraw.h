#pragma once

#include "bgfx/bgfx.h"
#include "common/debugdraw/debugdraw.h"
#include <cstdint>

struct ddRAII
{
	ddRAII(uint8_t _viewId);

	ddRAII();
};
