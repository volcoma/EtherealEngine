#pragma once
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/vector.hpp"

#define SERIALIZE_FUNCTION_NAME CEREAL_SERIALIZE_FUNCTION_NAME
#define SAVE_FUNCTION_NAME CEREAL_SAVE_FUNCTION_NAME
#define LOAD_FUNCTION_NAME CEREAL_LOAD_FUNCTION_NAME

namespace serialization
{
	using namespace cereal;
}

#define SERIALIZABLE(T) \
public:\
friend class serialization::access;\
template<typename Archive> friend void SAVE_FUNCTION_NAME(Archive & ar, T const &);\
template<typename Archive> friend void LOAD_FUNCTION_NAME(Archive & ar, T &);


#define SERIALIZE(cls) \
template<typename Archive> inline \
void SERIALIZE_FUNCTION_NAME(Archive & ar, cls & obj)

#define SAVE(cls) \
template<typename Archive> inline \
void SAVE_FUNCTION_NAME(Archive & ar, cls const & obj)

#define LOAD(cls) \
template<typename Archive> inline \
void LOAD_FUNCTION_NAME(Archive & ar, cls & obj)

#include "../logging/logging.h"
template<typename Archive, typename T>
inline bool try_save(Archive& ar, cereal::NameValuePair<T>&& t)
{
	try
	{
		ar(t);
	}
	catch (cereal::Exception e)
	{
		APPLOG_WARNING(e.what());
		return false;
	}
	return true;
}

template<typename Archive, typename T>
inline bool try_load(Archive& ar, cereal::NameValuePair<T>&& t)
{
	try
	{
		ar(t);
	}
	catch (cereal::Exception e)
	{
		APPLOG_WARNING(e.what());
		return false;
	}
	return true;
}


template<typename Archive, typename T>
inline bool try_serialize(Archive& ar, cereal::NameValuePair<T>&& t)
{
	try
	{
		ar(t);
	}
	catch (cereal::Exception e)
	{
		APPLOG_WARNING(e.what());
		return false;
	}
	return true;
}
