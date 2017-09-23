#include "debugdraw.h"

ddRAII::ddRAII(uint8_t _viewId)
{
    ddBegin(_viewId);
}

ddRAII::ddRAII()
{
    ddEnd();
}
