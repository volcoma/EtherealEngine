#ifndef REFLECTION_REGISTRATION_H
#define REFLECTION_REGISTRATION_H

#include "rttr/rttr_enable.h"
#include "rttr/registration_friend.h"

template <typename T>
extern void rttr_auto_register_reflection_function_t();
namespace rttr
{
namespace detail
{
template <typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer>
struct constructor_invoker;
}
}

#define RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE(cls)                                                          \
	friend void ::rttr_auto_register_reflection_function_t<cls>();                                           \
	RTTR_REGISTRATION_FRIEND

#define EXPAND(x) x
#define REFLECTABLE_VIRTUAL_IMPL(cls, ...)                                                                   \
	RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE(cls)                                                              \
	RTTR_ENABLE(__VA_ARGS__)                                                                                 \
                                                                                                             \
public:

#define REFLECTABLE(cls) RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE(cls)
#define REFLECTABLEV(...) EXPAND(REFLECTABLE_VIRTUAL_IMPL(__VA_ARGS__))
#endif // REFLECTION_REGISTRATION_H
