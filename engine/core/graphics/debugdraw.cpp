#include "debugdraw.h"

namespace gfx
{
dd_raii::dd_raii(view_id _viewId)
{
    encoder.begin(uint8_t(_viewId));
}

dd_raii::~dd_raii()
{
    encoder.end();
}
} // namespace gfx
