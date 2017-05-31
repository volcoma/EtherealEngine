#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#include "rttr/type.h"
#include "rttr/rttr_enable.h"
#include "rttr/rttr_cast.h"
#include "rttr/array_range.h"
#include "rttr/constructor.h"
#include "rttr/destructor.h"
#include "rttr/method.h"
#include "rttr/property.h"
#include "rttr/enumeration.h"
#include "rttr/registration.h"

#define _CAT_IMPL(a, b) a##b
#define _CAT(a, b) RTTR_CAT_IMPL(a, b)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) _CAT(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) _CAT(str, __LINE__)
#endif

template<typename T>
static void rttr_auto_register_reflection_function_t();

namespace
{
	template<typename T>
	struct rttr__auto__register__t
	{
		rttr__auto__register__t()
		{
			::rttr_auto_register_reflection_function_t<T>();
		}
	};
}

#define REFLECT(cls)                                           \
static const rttr__auto__register__t<cls> ANONYMOUS_VARIABLE(auto_register__); \
template<> inline void rttr_auto_register_reflection_function_t<cls>()

#endif // RTTR_REFLECTION_H_
