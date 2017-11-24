#pragma once

#include "../../math/math_includes.h"
#include "../../serialization/serialization.h"
#include "vector.hpp"

namespace cereal
{
template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat2x2<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat2x3<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
}
template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat2x4<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
	try_serialize(ar, cereal::make_nvp("col_3", obj[3]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat3x2<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat4x2<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat3x3<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat3x4<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
	try_serialize(ar, cereal::make_nvp("col_3", obj[3]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat4x3<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
}

template <typename Archive, typename T, math::qualifier P>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::tmat4x4<T, P>& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
	try_serialize(ar, cereal::make_nvp("col_3", obj[3]));
}

template <typename Archive>
inline void SERIALIZE_FUNCTION_NAME(Archive& ar, math::transform& obj)
{
	try_serialize(ar, cereal::make_nvp("col_0", obj[0]));
	try_serialize(ar, cereal::make_nvp("col_1", obj[1]));
	try_serialize(ar, cereal::make_nvp("col_2", obj[2]));
	try_serialize(ar, cereal::make_nvp("col_3", obj[3]));
}
}