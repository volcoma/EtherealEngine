#pragma once

//! A static, pre-execution object
/*! This class will create a single copy (singleton) of some
type and ensures that merely referencing this type will
cause it to be instantiated and initialized pre-execution.*/
template <class T>
class singleton
{
public:
	//-----------------------------------------------------------------------------
	//  Name : get_instance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static T & get_instance()
	{
		return create();
	}

private:
	//-----------------------------------------------------------------------------
	//  Name : instantiate ()
	/// <summary>
	/// Forces instantiation at pre-execution time
	/// </summary>
	//-----------------------------------------------------------------------------
	static void instantiate(T const &) {}

	//-----------------------------------------------------------------------------
	//  Name : create ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static T & create()
	{
		static T t;
		instantiate(t);
		return t;
	}

	//-----------------------------------------------------------------------------
	//  Name : singleton ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	singleton(singleton const & /*other*/) {}

	/// Internal instance
	static T& instance;
};

template <class T> T & singleton<T>::instance = singleton<T>::create();

template<void(*ctor)()>
struct static_initializer
{
	struct constructor { constructor() { ctor(); } };
	static constructor initializer;
};

template<void(*ctor)()>
typename static_initializer<ctor>::constructor static_initializer<ctor>::initializer;
