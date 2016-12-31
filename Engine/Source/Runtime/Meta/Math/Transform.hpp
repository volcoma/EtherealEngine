#pragma once

#include "core/math/math_includes.h"
#include "core/serialization/serialization.h"
#include "vector.hpp"

namespace cereal
{
	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat2x2<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1])
		);
	}


	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat2x3<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2])
		);
	}
	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat2x4<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2]),
			cereal::make_nvp("col_3", obj[3])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat3x2<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat4x2<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat3x3<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat3x4<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2]),
			cereal::make_nvp("col_3", obj[3])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat4x3<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2])
		);
	}

	template<typename Archive, typename T, math::precision P>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::tmat4x4<T, P>& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj[0]),
			cereal::make_nvp("col_1", obj[1]),
			cereal::make_nvp("col_2", obj[2]),
			cereal::make_nvp("col_3", obj[3])
		);
	}


	template<typename Archive>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, math::transform_t& obj)
	{
		ar(
			cereal::make_nvp("col_0", obj.matrix()[0]),
			cereal::make_nvp("col_1", obj.matrix()[1]),
			cereal::make_nvp("col_2", obj.matrix()[2]),
			cereal::make_nvp("col_3", obj.matrix()[3])
		);
	}
}

