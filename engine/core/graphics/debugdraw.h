#pragma once

#include <cstdint>
#include "bgfx/bgfx.h"
#include "common/debugdraw/debugdraw.h"
struct dd_raii
{
	dd_raii(uint8_t _viewId);

	dd_raii();
};
