#pragma once

#include "core/math/math_includes.h"
#include "core/serialization/serialization.h"

namespace cereal
{
	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec2<T, P>& obj)
	{
		ar(
			cereal::make_nvp("x", obj.x)
			, cereal::make_nvp("y", obj.y)
		);
	}


	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec3<T, P>& obj)
	{
		ar(
			cereal::make_nvp("x", obj.x)
			, cereal::make_nvp("y", obj.y)
			, cereal::make_nvp("z", obj.z)

		);
	}


	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tvec4<T, P>& obj)
	{
		ar(
			cereal::make_nvp("x", obj.x)
			, cereal::make_nvp("y", obj.y)
			, cereal::make_nvp("z", obj.z)
			, cereal::make_nvp("w", obj.w)
		);
	}

	template<typename Archive>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::color& obj)
	{
		ar(
			cereal::make_nvp("r", obj.Value.r)
			, cereal::make_nvp("g", obj.Value.g)
			, cereal::make_nvp("b", obj.Value.b)
			, cereal::make_nvp("a", obj.Value.a)
		);
	}
}

