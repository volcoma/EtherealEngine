#pragma once

#include "../../math/math_includes.h"
#include "../../serialization/serialization.h"

namespace cereal
{
template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tquat<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("x", obj.x));
	try_serialize(ar, cereal::make_nvp("y", obj.y));
    try_serialize(ar, cereal::make_nvp("z", obj.z));
    try_serialize(ar, cereal::make_nvp("w", obj.w));
}

}
