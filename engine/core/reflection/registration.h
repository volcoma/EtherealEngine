#ifndef _REFLECTION_REGISTRATION_H_
#define _REFLECTION_REGISTRATION_H_

#include "rttr/rttr_enable.h"

template<typename T>
static void rttr_auto_register_reflection_function_t();
namespace rttr
{
	namespace detail
	{
		template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer>
		struct constructor_invoker;
	}
}

#define RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE() template<typename T> friend void ::rttr_auto_register_reflection_function_t();   \
                                 template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> \
                                 friend struct rttr::detail::constructor_invoker;


#define EXPAND(x) x
#define REFLECTABLE_IMPL(cls, ...)\
RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE()\
RTTR_ENABLE(__VA_ARGS__)\
public:

#define REFLECTABLE(...) EXPAND(REFLECTABLE_IMPL(__VA_ARGS__))


#endif // _REFLECTION_REGISTRATION_H_
