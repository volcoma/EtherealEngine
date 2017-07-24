#ifndef REFLECTION_H
#define REFLECTION_H

#include "rttr/array_range.h"
#include "rttr/constructor.h"
#include "rttr/destructor.h"
#include "rttr/enumeration.h"
#include "rttr/method.h"
#include "rttr/property.h"
#include "rttr/registration.h"
#include "rttr/rttr_cast.h"
#include "rttr/rttr_enable.h"
#include "rttr/type.h"

#define CAT_IMPL_(a, b) a##b
#define CAT_(a, b) CAT_IMPL_(a, b)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CAT_(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CAT_(str, __LINE__)
#endif

template <typename T>
static void rttr_auto_register_reflection_function_t();

namespace
{
template <typename T>
struct rttr__auto__register__t
{
	rttr__auto__register__t()
	{
		::rttr_auto_register_reflection_function_t<T>();
	}
};
}

#define REFLECT_INLINE(cls)                                                                                  \
	static const rttr__auto__register__t<cls> ANONYMOUS_VARIABLE(auto_register__);                           \
	template <>                                                                                              \
	inline void rttr_auto_register_reflection_function_t<cls>()

#define REFLECT_EXTERN(cls) // template<> void rttr_auto_register_reflection_function_t<cls>();

#define REFLECT(cls)                                                                                         \
	static const rttr__auto__register__t<cls> ANONYMOUS_VARIABLE(auto_register__);                           \
	template <>                                                                                              \
	void rttr_auto_register_reflection_function_t<cls>()

#endif // RTTR_REFLECTION_H_
