#include "debugdraw.h"

namespace gfx
{ 
dd_raii::dd_raii(view_id _viewId)
{
	ddBegin(uint8_t(_viewId));
}

dd_raii::~dd_raii()
{
	ddEnd();
}
}
