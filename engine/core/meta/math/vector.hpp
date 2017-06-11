#pragma once

#include "../../math/math_includes.h"
#include "../../serialization/serialization.h"

namespace cereal
{
	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec2<T, P>& obj)
	{
		try_serialize(ar, cereal::make_nvp("x", obj.x));
		try_serialize(ar, cereal::make_nvp("y", obj.y));
	}


	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec3<T, P>& obj)
	{
		try_serialize(ar, cereal::make_nvp("x", obj.x));
		try_serialize(ar, cereal::make_nvp("y", obj.y));
		try_serialize(ar, cereal::make_nvp("z", obj.z));
	}


	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec4<T, P>& obj)
	{
		try_serialize(ar, cereal::make_nvp("x", obj.x));
		try_serialize(ar, cereal::make_nvp("y", obj.y));
		try_serialize(ar, cereal::make_nvp("z", obj.z));
		try_serialize(ar, cereal::make_nvp("w", obj.w));
	}

	template<typename Archive>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::color& obj)
	{
		try_serialize(ar, cereal::make_nvp("r", obj.value.r));
		try_serialize(ar, cereal::make_nvp("g", obj.value.g));
		try_serialize(ar, cereal::make_nvp("b", obj.value.b));
		try_serialize(ar, cereal::make_nvp("a", obj.value.a));
	}
}