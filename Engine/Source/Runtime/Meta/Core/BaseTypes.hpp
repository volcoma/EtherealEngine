#pragma once

#include "Core/common/basetypes.hpp"
#include "Core/serialization/serialization.h"

namespace cereal
{
	template<typename Archive, typename T>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, Rect<T>& obj)
	{
		ar(
			cereal::make_nvp("left", obj.left),
			cereal::make_nvp("top", obj.top),
			cereal::make_nvp("right", obj.right),
			cereal::make_nvp("bottom", obj.bottom)
		);
	}


	template<typename Archive, typename T>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, Size<T>& obj)
	{
		ar(
			cereal::make_nvp("width", obj.width),
			cereal::make_nvp("height", obj.height)
		);
	}

	template<typename Archive, typename T>
	inline void SERIALIZE_FUNCTION_NAME(Archive & ar, Point<T>& obj)
	{
		ar(
			cereal::make_nvp("x", obj.x),
			cereal::make_nvp("y", obj.y)
		);
	}
}

