#pragma once

#include "graphics.h"
//
#include "debugdraw/debugdraw.h"
#include <cstdint>

namespace gfx
{
struct dd_raii
{
	dd_raii(view_id _viewId);

	~dd_raii();

	DebugDrawEncoder encoder;
};
}


