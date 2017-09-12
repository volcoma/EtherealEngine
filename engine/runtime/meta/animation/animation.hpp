#pragma once

#include "../../animation/animation.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

SAVE_EXTERN(animation);
LOAD_EXTERN(animation);

SAVE_EXTERN(node_animation);
LOAD_EXTERN(node_animation);

namespace cereal
{
template <typename Archive, typename T>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, node_animation::key<T>& obj)
{
	try_serialize(ar, cereal::make_nvp("time", obj.time));
	try_serialize(ar, cereal::make_nvp("value", obj.value));

}
}
