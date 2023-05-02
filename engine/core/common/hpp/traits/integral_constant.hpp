#pragma once
#include <type_traits>

namespace hpp
{
/// BOOL CONSTANT
template <bool B>
using bool_constant = std::integral_constant<bool, B>;
}
