#pragma once

#ifndef CORE_CHECKED_DELETE_H
#define CORE_CHECKED_DELETE_H

// verify that types are complete for increased safety
template<typename T> 
inline void checked_delete(T*& x)
{
	static_assert(sizeof(T), "Trying to delete a pointer to an incomplete type.");
    delete x;
	x = nullptr;
}

template<typename T> 
inline void checked_array_delete(T*& x)
{
	static_assert(sizeof(T), "Trying to delete a pointer to an incomplete type.");
    delete [] x;
	x = nullptr;
}

template<typename T> 
struct checked_deleter
{
    void operator()(T*& x) const
    {
        checked_delete(x);
    }
};

template<typename T> 
struct checked_array_deleter
{
    void operator()(T*& x) const
    {
        checked_array_delete(x);
    }
};

#endif  // #ifndef CORE_CHECKED_DELETE_H
