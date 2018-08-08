#pragma once

#include <stdexcept>

namespace audio
{
    struct exception : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };
}
