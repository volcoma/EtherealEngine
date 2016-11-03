#pragma once


template<void(*ctor)()>
struct StaticInitializer
{
	struct constructor { constructor() { ctor(); } };
	static constructor initializer;
};

template<void(*ctor)()>
typename StaticInitializer<ctor>::constructor StaticInitializer<ctor>::initializer;


//! A static, pre-execution object
/*! This class will create a single copy (singleton) of some
type and ensures that merely referencing this type will
cause it to be instantiated and initialized pre-execution.*/
template <class T>
class Singleton
{
public:
	static T & getInstance()
	{
		return create();
	}

private:
	//! Forces instantiation at pre-execution time
	static void instantiate(T const &) {}
	static T & create()
	{
		static T t;
		instantiate(t);
		return t;
	}

	Singleton(Singleton const & /*other*/) {}
	static T & instance;
};

template <class T> T & Singleton<T>::instance = Singleton<T>::create();