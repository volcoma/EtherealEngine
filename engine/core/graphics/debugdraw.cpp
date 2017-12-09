#include "debugdraw.h"

dd_raii::dd_raii(uint8_t _viewId)
{
	ddBegin(_viewId);
}

dd_raii::dd_raii()
{
	ddEnd();
}
