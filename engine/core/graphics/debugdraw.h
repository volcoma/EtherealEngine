#pragma once

#include "bgfx/bgfx.h"
#include "common/debugdraw/debugdraw.h"
#include <cstdint>
struct dd_raii
{
	dd_raii(uint8_t _viewId);

	dd_raii();
};
